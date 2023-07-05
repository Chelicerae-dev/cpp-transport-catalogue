#include "domain.h"

namespace transport_catalogue::detail {

    OutputQuery::OutputQuery(int64_t id, std::string type)
    : id(id), type(type) {}

    OutputQuery::OutputQuery(int64_t id, std::string type, std::string name)
        : id(id), type(type), name(name) {}

    bool Bus::operator<(const Bus& other) {
        return this->name < other.name;
    }

    Weight Weight::operator+(const Weight& other) const {
        return {value + other.value, is_wait, span + other.span, name};
    }
    bool Weight::operator<(const Weight& other) const {
        return value < other.value && !std::lexicographical_compare(std::string(name).begin(), std::string(name).end(), std::string(other.name).begin(), std::string(other.name).end());
    }
    bool Weight::operator>(const Weight& other) const {
        return value > other.value && std::lexicographical_compare(std::string(name).begin(), std::string(name).end(), std::string(other.name).begin(), std::string(other.name).end());
    }
}
