#include "input_reader.h"

namespace transport_catalogue::input {
    InputReader::InputReader(std::istream& is, backend::TransportCatalogue& transport_catalogue) {
        std::string temp;
        int queries_count = 0;
        std::getline(is, temp);
        queries_count = std::stoi(temp);
        temp.clear();
        using namespace std::string_literals;
        while(queries_count != 0) {
            --queries_count;
            std::getline(is, temp);
            //добавляем sv в виде костыля на данный момент (на случай длинных запросов)
            //мало чем отличается от копирования, разве что экономим на первом разделении строки)
            std::string_view temp_string(temp);
            int64_t pos = 0;// = temp_string.find_first_not_of(" ");
            int64_t type_ender = temp_string.find(' ', pos);
            std::string_view type_text = temp_string.substr(pos, type_ender - pos);
            pos = type_ender + 1;
            int64_t delimiter = temp_string.find(':', pos);

            //processing name and contents
            if(type_text == "Stop"s) {
                ParseStop(transport_catalogue, temp_string, pos, delimiter);

            } else if(type_text == "Bus"s) {
                ParseBus(temp_string, pos, delimiter);
            }
            temp.clear();
        }
        if(queries_count != 0) {
            throw std::length_error("Input contains less lines than expected!"s);
        }

        //Добавляем дистанции
        std::for_each(stops_query_.begin(), stops_query_.end(), [&transport_catalogue](auto& stop) {
            detail::Stop* stop_ptr = transport_catalogue.GetStop(stop.name);
            transport_catalogue.AddStopDistances(stop_ptr, stop.distances);
        });

        //Добавляем автобусы в базу
        std::for_each(buses_query_.begin(), buses_query_.end(), [&transport_catalogue] (auto& this_bus) {
            transport_catalogue.AddBus(transport_catalogue.MakeBus(this_bus.name, this_bus.stops, this_bus.is_looped));
        });
    }

    void InputReader::ParseStop(backend::TransportCatalogue& transport_catalogue, std::string_view temp_string, int64_t pos, int64_t delimiter) {
        //получаем имя остановки
        std::string_view name = temp_string.substr(pos, delimiter - pos);
        //двигаемся за двоеточие и получаем первую координату
        pos = temp_string.find_first_not_of(' ', delimiter + 1);
        delimiter = temp_string.find(',', pos);
        std::string_view longitude_str = temp_string.substr(pos, delimiter);
        //двигаемся дальше и получаем вторую координату
        pos = temp_string.find_first_not_of(' ', delimiter + 1);
        delimiter = temp_string.find(',', pos);
        std::string_view latitude_str = temp_string.substr(pos, delimiter);
        //создаём строки и конвертируем в дабл
        double longitude = std::stod(std::string(longitude_str));
        double latitude = std::stod(std::string(latitude_str));
        //создаём остановку
        transport_catalogue.AddStop({std::string(name), {longitude, latitude}});

        //парсим дистанции если нашлась запятая после координат)
        if(delimiter != temp_string.npos) {
            ParseDistances(name, temp_string.substr(delimiter + 1, temp_string.size()));
        }
    }

    void InputReader::ParseDistances(std::string_view name, std::string_view line) {
        std::map<std::string, int> distances;
        int64_t pos = line.find_first_not_of(' ');
        int64_t delimiter = line.find(',', pos);
        while(delimiter != line.npos) {
            std::string_view sub_line = line.substr(pos, delimiter - pos);
            int64_t space = sub_line.find(' ');
            std::string_view distance = sub_line.substr(0, space - 1);
            //пропускаем to
            space = sub_line.find(' ', sub_line.find_first_not_of(' ', space + 1));
            std::string_view stop_name = sub_line.substr(sub_line.find_first_not_of(' ', space + 1), sub_line.size());
            //пишем во временную мапу
            distances[std::string(stop_name)] = std::stoi(std::string(distance));
            //переносим границы
            pos = line.find_first_not_of(' ', delimiter + 2);
            delimiter = line.find(',', pos);
        }
        //добавляем последнюю дистанцию
        std::string_view sub_line = line.substr(pos, line.size() - pos);
        int64_t space = sub_line.find(' ');
        std::string_view distance = sub_line.substr(0, space - 1);
        //пропускаем to
        space = sub_line.find(' ', sub_line.find_first_not_of(' ', space + 1));
        std::string_view stop_name = sub_line.substr(sub_line.find_first_not_of(' ', space + 1), sub_line.size());
        //пишем во временную мапу
        distances[std::string(stop_name)] = std::stoi(std::string(distance));

        //добавляем в очередь
        stops_query_.push_back({std::string(name), std::move(distances)});
    }

    void InputReader::ParseBus(std::string_view temp_string, int64_t pos, int64_t delimiter) {
        std::vector<std::string> stops;
        bool is_looped = false;
        std::string_view name = temp_string.substr(pos, delimiter - pos);
        std::string_view stop_names = temp_string.substr(delimiter + 1, temp_string.npos);
        pos = 1;
        char stops_delimiter;
        if(stop_names.find('>') != stop_names.npos) {
            is_looped = true;
            stops_delimiter = '>';
        } else {
            stops_delimiter = '-';
        }
        int64_t new_pos = stop_names.find(stops_delimiter, pos);
        while(new_pos != -1) {
            stops.push_back(std::string(stop_names.substr(pos, new_pos - pos - 1)));
            pos = new_pos + 2;
            new_pos = stop_names.find(stops_delimiter, pos);
        }
        stops.push_back(std::string(stop_names.substr(pos, stop_names.size())));
        buses_query_.push_back({std::string(name), stops, is_looped});
    }

}
