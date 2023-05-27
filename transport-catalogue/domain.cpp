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
/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области
 * (domain) вашего приложения и не зависят от транспортного справочника. Например Автобусные
 * маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */
