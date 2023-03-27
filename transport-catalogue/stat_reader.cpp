#include "stat_reader.h"

namespace transport_catalogue::output {

    StatReader::StatReader(std::istream& input, backend::TransportCatalogue& transport_catalogue) {
        std::string temp_string;
        int queries_count = 0;
        std::getline(input, temp_string);
        queries_count = std::stoi(temp_string);
        temp_string.clear();
        using namespace std::string_literals;
        BusWordSize_ = "Bus"s.size();
        StopWordSize_ = "Stop"s.size();
        while(queries_count != 0) {
            --queries_count;
            std::getline(input, temp_string);
            if(int64_t pos =temp_string.find("Bus"s) != temp_string.npos) {
                //вместо именованных переменных в методах явным образом прибавляю длину строки (Bus или Stop) к позиции
                pos += BusWordSize_;
                ParseBusQuery(transport_catalogue, temp_string, pos);
            } else if(int64_t pos =temp_string.find("Stop"s) != temp_string.npos) {
                //вместо именованных переменных в методах явным образом прибавляю длину строки (Bus или Stop) к позиции
                pos += StopWordSize_;
                ParseStopQuery(transport_catalogue, temp_string, pos);
            }
            temp_string.clear();
        }
    }

    void StatReader::ParseBusQuery(backend::TransportCatalogue& transport_catalogue, std::string_view temp_string, int64_t pos) {
        using namespace std::string_literals;
        pos = temp_string.find_first_not_of(' ', pos);
        std::string_view bus_name = temp_string.substr(pos, temp_string.size());
        detail::Bus* bus = transport_catalogue.GetBus(bus_name);
        if(bus != nullptr) {
            bus_queries_.push_back(transport_catalogue.GetBusInfo(bus));
            queries_.push_back({std::string(bus_name), bus_queries_.size() - 1, detail::QueryType::BusQuery, true});
        } else {
            queries_.push_back({std::string(bus_name), 0, detail::QueryType::BusQuery, false});
        }
    }

    void StatReader::ParseStopQuery(backend::TransportCatalogue& transport_catalogue, std::string_view temp_string, int64_t pos) {
        pos = temp_string.find_first_not_of(' ', pos);
        std::string_view stop_name = temp_string.substr(pos, temp_string.size());
        detail::Stop* stop = transport_catalogue.GetStop(stop_name);
        if(stop != nullptr) {
            stop_queries_.push_back(transport_catalogue.GetStopInfo(stop));
            queries_.push_back({std::string(stop_name), stop_queries_.size() - 1, detail::QueryType::StopQuery, true});
        } else {
            queries_.push_back({std::string(stop_name), 0, detail::QueryType::StopQuery, false});
        }
    }

    void StatReader::PrintQueries() {
        using namespace std::string_literals;
        std::for_each(queries_.begin(), queries_.end(), [this](const auto& query){
            if(query.type == detail::QueryType::BusQuery) {
                if(!query.exists) {
                    std::cout << "Bus "s << query.name << ": not found"s << std::endl;
                } else {
                    detail::BusInfo& bus = bus_queries_[query.place];
                    std::cout << "Bus "s << bus.name << ": "s << bus.stop_count <<" stops on route, "s << bus.unique_stop_count << " unique stops, "s
                              << bus.length << " route length, "s << std::setprecision(6) << bus.curvature << " curvature"s << std::endl;
                }
            } else if(query.type == detail::QueryType::StopQuery) {
                if(!query.exists) {
                    std::cout << "Stop "s << query.name << ": not found"s << std::endl;
                } else {
                    detail::StopInfo& stop = stop_queries_[query.place];
                    std::cout << "Stop "s << stop.name << ": "s;
                    if(stop.buses.size() == 0) {
                        std::cout << "no buses"s << std::endl;
                    } else {
                        std::cout << "buses "s;
                        std::vector<std::string> bus_names;
                        std::for_each(stop.buses.begin(), stop.buses.end(), [&bus_names](auto bus) {
                            bus_names.push_back(std::string(bus));
                        });
                        for(size_t i = 0; i < bus_names.size(); ++i) {
                            if(i == bus_names.size() - 1) {
                                std::cout << bus_names[i] << std::endl;
                            } else {
                                std::cout << bus_names[i] << ' ';
                            }
                        }
                    }
                }
            }

        });
    }

}
