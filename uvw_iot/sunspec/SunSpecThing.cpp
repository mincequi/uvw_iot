#include "SunSpecThing.h"

#include <cmath>

#include <uvw_iot/util/Util.h>
#include <uvw_net/sunspec/SunSpecTypes.h>

namespace uvw_iot {
namespace sunspec {

using namespace uvw_net::sunspec;

SunSpecThing::SunSpecThing(SunSpecClientPtr client) :
    _client(client) {
    for (const auto& kv : client->models()) {
        auto m = magic_enum::enum_cast<SunSpecModel::Id>(kv.first);
        if (m.has_value()) {
            switch (m.value()) {
                case SunSpecModel::Id::InverterSinglePhase:
                case SunSpecModel::Id::InverterThreePhase:
                case SunSpecModel::Id::InverterSplitPhase:
                    _type = ThingType::SolarInverter;
                    break;
                case SunSpecModel::Id::MeterWyeConnectThreePhase:
                    _type = ThingType::SmartMeter;
                    break;
                default:
                    break;
            }
        }
        if (_type != ThingType::Unknown) break;
    }

    _client->on<uvw::close_event>([this](const auto&, const auto&) {
        close();
    });
    _client->on<uvw::error_event>([this](const auto&, const auto&) {
        ++_errorCount;
        if (_errorCount > 5) close();
    });
    _client->on<uvw_net::modbus::ModbusException>([this](const auto&, const auto&) {
        ++_errorCount;
        if (_errorCount > 5) close();
    });
    _client->on<SunSpecModel>([this](const SunSpecModel& model, const auto&) {
        _errorCount = 0;

        ThingPropertyMap properties;
        for (const auto& v : model.values()) {
            switch (v.first) {
            case DataPoint::totalActiveAcPower:
                properties.set<ThingPropertyKey::power>(std::get<int>(v.second));
                break;
            case DataPoint::operatingStatus:
                properties.set<ThingPropertyKey::status>((int)std::get<InverterOperatingStatus>(v.second));
                break;
            case DataPoint::dc: {
                auto blockArray = std::get<std::vector<SunSpecBlock<double>>>(v.second);
                std::vector<int> dcPowers(blockArray.size());
                for (size_t i = 0; i < blockArray.size(); ++i) {
                    dcPowers[i] = round(blockArray[i].data().at(DataPoint::power));
                }
                properties.set<ThingPropertyKey::dcPower>(dcPowers);
                break;
            }
            default:
                break;
            }
        }
        publish(properties);
    });
}

SunSpecThing::~SunSpecThing() {
}

const std::string& SunSpecThing::id() const {
    return _client->sunSpecId();
}

ThingType SunSpecThing::type() const {
    return _type;
}

void SunSpecThing::disconnect() {
    _client->disconnect();
}

void SunSpecThing::fetchProperties() {
    // TODO: implement real timeout. This only increments the error count.
    ++_errorCount;
    if (_errorCount > 5) close();

    for (const auto& kv : _client->models()) {
        switch (kv.first) {
        case SunSpecModel::Id::InverterSinglePhase:
        case SunSpecModel::Id::InverterThreePhase:
        case SunSpecModel::Id::InverterSplitPhase:
        case SunSpecModel::Id::MeterWyeConnectThreePhase:
        case SunSpecModel::Id::InverterMpptExtension:
            _client->readModel(kv.first);
            break;
        default:
            break;
        }
    }
}

} // namespace sunspec
} // namespace uvw_iot
