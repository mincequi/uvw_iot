#include "SunSpecThing.h"

#include <uvw_iot/common/Util.h>
#include <uvw_net/sunspec/SunSpecTypes.h>

namespace uvw_iot {
namespace sunspec {

using namespace uvw_iot::common;
using namespace uvw_net::sunspec;

SunSpecThing::SunSpecThing(SunSpecClientPtr client) :
    _client(client) {

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

        ThingPropertyMap props;
        for (const auto& v : model.values()) {
            switch (v.first) {
            case DataPoint::totalActiveAcPower:
                props[ThingPropertyKey::power] = (int16_t)Util::divAndRound(std::get<int32_t>(v.second), 10);
                break;
            case DataPoint::operatingStatus:
                props[ThingPropertyKey::status] = (int16_t)std::get<InverterOperatingStatus>(v.second);
            default:
                break;
            }
        }
        publish(props);
    });
}

const std::string& SunSpecThing::id() const {
    return _client->sunSpecId();
}

void SunSpecThing::getProperties() {
    // TODO: implement real timeout. This only increments the error count.
    ++_errorCount;
    if (_errorCount > 5) close();

    for (const auto& kv : _client->models()) {
        switch (kv.first) {
        case SunSpecModel::Id::InverterSinglePhase:
        case SunSpecModel::Id::InverterThreePhase:
        case SunSpecModel::Id::InverterSplitPhase:
        case SunSpecModel::Id::MeterWyeConnectThreePhase:
            _client->readModel(kv.first);
            break;
        default:
            break;
        }
    }
}

} // namespace sunspec
} // namespace uvw_iot
