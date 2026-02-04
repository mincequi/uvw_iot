#pragma once

#include <array>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

namespace uvw_iot {

// Check this for reference: https://github.com/bluetoother/bipso/wiki/BIPSO-Specification
enum class ThingPropertyKey : size_t {
    // custom properties - generic
    timestamp,

    // custom properties - things
    name,
    pinned,
    status,
    //events,
    //type,
    //icon,

    // custom properties - strategies
    offset,
    thing_interval,
    //time_constant,
    countdown,

    // custom properties - site
    pv_power,
    grid_power,
    short_term_grid_power,
    long_term_grid_power,

    // ipso properties
    //generic_sensor,
    digitalInput,
    digitalOutput,
    analogInput,
    analogOutput,
    temperature,
    power,
    actuation,
    power_control,

    phases,
    next_phases,

    // ipso properties - not meant for export
    voltage, // Only object, no reusable object
    current, // Only object, no reusable object

    //frequency,
    //depth,
    percentage,
    //altitude,
    //load,
    //pressure,
    //loudness,
    //concentration,
    //acidity,
    //conductivity,

    //distance,
    energy, // [0.1 kWh]
    //direction,
    multistateSelector,

    count
};

using ThingPropertyValue = std::variant<bool, int, std::string, std::array<int, 3>, std::vector<int>, std::vector<bool>>;
using ThingProperty = std::pair<ThingPropertyKey, ThingPropertyValue>;
//using ThingPropertyMap = std::map<ThingPropertyKey, ThingPropertyValue>;
//using ElectricValue = std::variant<int, std::array<int, 3>>;

// Key → value type mapping
template<ThingPropertyKey K> struct PropertyType;
template<> struct PropertyType<ThingPropertyKey::timestamp>  { using type = int; };
template<> struct PropertyType<ThingPropertyKey::name>  { using type = std::string; };
template<> struct PropertyType<ThingPropertyKey::pinned>  { using type = bool; };
template<> struct PropertyType<ThingPropertyKey::status>  { using type = int; };
template<> struct PropertyType<ThingPropertyKey::offset>  { using type = int; };
template<> struct PropertyType<ThingPropertyKey::thing_interval>  { using type = int; };
template<> struct PropertyType<ThingPropertyKey::countdown>  { using type = int; };
template<> struct PropertyType<ThingPropertyKey::pv_power>  { using type = int; };
template<> struct PropertyType<ThingPropertyKey::grid_power>  { using type = int; };
template<> struct PropertyType<ThingPropertyKey::short_term_grid_power>  { using type = int; };
template<> struct PropertyType<ThingPropertyKey::long_term_grid_power>  { using type = int; };

template<> struct PropertyType<ThingPropertyKey::digitalInput>  { using type = std::vector<bool>; };
template<> struct PropertyType<ThingPropertyKey::digitalOutput>  { using type = std::vector<bool>; };
template<> struct PropertyType<ThingPropertyKey::analogInput>  { using type = std::vector<int>; };
template<> struct PropertyType<ThingPropertyKey::analogOutput>  { using type = std::vector<int>; };
template<> struct PropertyType<ThingPropertyKey::temperature>  { using type = double; };
template<> struct PropertyType<ThingPropertyKey::power> { using type = int; };
template<> struct PropertyType<ThingPropertyKey::actuation> { using type = bool; };
template<> struct PropertyType<ThingPropertyKey::power_control> { using type = bool; };
template<> struct PropertyType<ThingPropertyKey::phases>  { using type = int; };
template<> struct PropertyType<ThingPropertyKey::next_phases>  { using type = int; };
template<> struct PropertyType<ThingPropertyKey::voltage>  { using type = std::array<int, 3>; };
template<> struct PropertyType<ThingPropertyKey::current>  { using type = int; };
template<> struct PropertyType<ThingPropertyKey::percentage>  { using type = std::vector<int>; };
template<> struct PropertyType<ThingPropertyKey::energy>  { using type = int; };
template<> struct PropertyType<ThingPropertyKey::multistateSelector>  { using type = std::vector<bool>; };


template<ThingPropertyKey K> using PropertyValueT = typename PropertyType<K>::type;

// Helper: generate a tuple of optional types from PropertyKey
template<size_t... Is>
static auto makePropertyTuple(std::index_sequence<Is...>) {
    return std::tuple<std::optional<PropertyValueT<static_cast<ThingPropertyKey>(Is)>>...>{};
}

struct ThingPropertyMap {
    template<ThingPropertyKey K>
    void set(const PropertyValueT<K>& v) {
        std::get<static_cast<size_t>(K)>(_values) = v;
    }

    template<typename T>
    void set(ThingPropertyKey key, T&& value) {
        static constexpr auto table = makeSetTable<T>();
        const auto idx = static_cast<std::size_t>(key);
        table[idx](*this, std::forward<T>(value));
    }

    template<ThingPropertyKey K>
    void reset() {
        std::get<static_cast<size_t>(K)>(_values).reset();
    }

    template<ThingPropertyKey K>
    std::optional<PropertyValueT<K>> get() const {
        return std::get<static_cast<size_t>(K)>(_values);
    }

    template<ThingPropertyKey K>
    bool contains() const {
        return std::get<static_cast<size_t>(K)>(_values).has_value();
    }

    template<typename Func>
    void forEach(Func&& f) const {
        forEachImpl(std::forward<Func>(f), std::make_index_sequence<static_cast<size_t>(ThingPropertyKey::count)>{});
    }

    template<typename OtherMap>
    void assignFrom(const OtherMap& src) {
        assignFromImpl(src, std::make_index_sequence<static_cast<size_t>(ThingPropertyKey::count)>{});
    }

    template<ThingPropertyKey K, typename Func>
    const ThingPropertyMap& on(Func&& f) const {
        const auto& opt = std::get<static_cast<size_t>(K)>(_values);
        if (opt)
            f(*opt);

        return *this;
    }

    bool empty() const {
        return emptyImpl(std::make_index_sequence<std::tuple_size_v<TupleT>>{});
    }

private:
    using TupleT = decltype(makePropertyTuple(std::make_index_sequence<static_cast<size_t>(ThingPropertyKey::count)>{}));

    template<typename T>
    static constexpr auto makeSetTable() {
        return makeSetTableImpl<T>(std::make_index_sequence<
                                   static_cast<std::size_t>(ThingPropertyKey::count)>{});
    }

    template<typename T, std::size_t... Is>
    static constexpr auto makeSetTableImpl(std::index_sequence<Is...>) {
        using Fn = void(*)(ThingPropertyMap&, T&&);

        return std::array<Fn, sizeof...(Is)>{
            // one lambda per enum index Is
            +[](ThingPropertyMap& m, T&& v) {
                constexpr ThingPropertyKey K = static_cast<ThingPropertyKey>(Is);
                using Expected = typename PropertyType<K>::type;

                if constexpr (std::is_convertible_v<T, Expected>) {
                    // only compiled if T convertible to Expected
                    m.template set<K>(static_cast<Expected>(std::forward<T>(v)));
                } else {
                    // no-op for non-convertible types
                }
            }...
        };
    }

    template<ThingPropertyKey K, typename T>
    void dispatchSetOne(T&& value) {
        using Expected = typename PropertyType<K>::type;

        // ensure convertibility
        static_assert(std::is_convertible_v<T, Expected>,
                      "Value not convertible to expected PropertyType<K>::type");

        set<K>(static_cast<Expected>(value));
    }

    template<typename T, size_t... Is>
    void dispatchSet(ThingPropertyKey key, T&& value, std::index_sequence<Is...>) {
        // fold expression but wrapped in a lambda to avoid instantiation
        (..., (key == static_cast<ThingPropertyKey>(Is)
                   ? (dispatchSetOne<static_cast<ThingPropertyKey>(Is)>(std::forward<T>(value)), 0)
                   : 0
               ));
    }

    template<typename Func, size_t... Is>
    void forEachImpl(Func&& f, std::index_sequence<Is...>) const {
        ( ( std::get<Is>(_values) ? f(static_cast<ThingPropertyKey>(Is), *std::get<Is>(_values)) : void() ), ... );
    }

    template<typename OtherMap, size_t... Is>
    void assignFromImpl(const OtherMap& src, std::index_sequence<Is...>) {
        ( assignOne<static_cast<ThingPropertyKey>(Is)>(src), ... );
    }

    template<ThingPropertyKey K, typename OtherMap>
    void assignOne(const OtherMap& src) {
        if (auto v = src.template get<K>())
            set<K>(*v);
    }

    template<std::size_t... Is>
    bool emptyImpl(std::index_sequence<Is...>) const {
        return (!(std::get<Is>(_values).has_value()) && ...);
    }

    // Automatically generate the tuple for all keys
    TupleT _values;
};

} // namespace uvw_iot
