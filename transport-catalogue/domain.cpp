#include "domain.h"

namespace transport_catalogue::detail {

    OutputQuery::OutputQuery(int64_t id, std::string type)
    : id(id), type(type) {}

    OutputQuery::OutputQuery(int64_t id, std::string type, std::string name)
        : id(id), type(type), name(name) {}

    bool Bus::operator<(const Bus& other) {
        return this->name < other.name;
    }
}
