    #include "transport_catalogue.h"
    #include "geo.h"

    namespace transport_catalogue::backend {

        void TransportCatalogue::AddStop(detail::Stop stop) {
            stops_.push_back(stop);
            auto this_stop = &stops_.back();
            stopname_to_stop_[std::string_view(this_stop->name)] = this_stop;
        }

        void TransportCatalogue::AddBus(detail::Bus bus) {
            buses_.push_back(bus);
            auto this_bus = &buses_.back();
            busname_to_bus_[std::string_view(this_bus->name)] = this_bus;
            std::for_each(this_bus->stops.begin(), this_bus->stops.end(), [this_bus](detail::Stop* stop) {
                stop->buses.insert(this_bus);
            });
        }


        void TransportCatalogue::AddStopDistances(detail::Stop* stop, std::map<std::string, int> other_stops) {
            std::for_each(other_stops.begin(), other_stops.end(), [stop, this](const auto& other_stop_pair) {
                //берём указатель на вторую ("другую") остановку и расстояние в отдельные переменные
                auto other_stop = GetStop(other_stop_pair.first);
                int distance = other_stop_pair.second;
                distances_[stop][other_stop] = distance;
                if(distances_.count(other_stop) == 0 || distances_.at(other_stop).count(stop) == 0) {
                    distances_[other_stop][stop] = distance;
                }
            });
        }

        int TransportCatalogue::GetStopDistance(detail::Stop* first_stop, detail::Stop* second_stop) {
            if(distances_.count(first_stop) == 0) throw std::logic_error("No such stop in distances");
            if(distances_.at(first_stop).count(second_stop) != 0) {
                return distances_.at(first_stop).at(second_stop);
            } else {
                return 0;
            }
        }


        detail::Stop* TransportCatalogue::GetStop(std::string_view stop_name){

            return stopname_to_stop_.count(stop_name) != 0 ? stopname_to_stop_.at(stop_name) : nullptr;
        }

        detail::Bus* TransportCatalogue::GetBus(std::string_view bus_name) {
            return busname_to_bus_.count(bus_name) != 0 ? busname_to_bus_.at(bus_name) : nullptr;
        }

        detail::Bus TransportCatalogue::MakeBus(std::string& bus_name, std::vector<std::string>& stop_names, bool is_looped) {
            detail::Bus result;
            result.name = std::move(bus_name);
            std::for_each(stop_names.begin(), stop_names.end(), [this, &result](std::string_view stop){
                result.stops.push_back(GetStop(stop));
            });
            result.is_looped = is_looped;
            return result;
        }

        detail::Bus TransportCatalogue::MakeBus(detail::BusCreationQuery query) {
            return MakeBus(query.name, query.stops, query.is_looped);
        }

        detail::BusInfo TransportCatalogue::GetBusInfo(detail::Bus* original) {
            detail::Bus bus = *original;
            if(!bus.is_looped) {
                std::for_each(original->stops.rbegin() + 1, original->stops.rend(), [this, &bus](detail::Stop* stop){
                    bus.stops.push_back(stop);
                });
            }
            std::string_view name = bus.name;
            int stop_count = bus.stops.size();
            int length = 0;
            double length_geo = 0;
            //задаём невозможные (больше максимума) координаты
            double impossible_longitude = 100;
            double impossible_latitude = 200;
            geo::Coordinates impossible_coordinates = {impossible_longitude, impossible_latitude};
            geo::Coordinates temp_coords = impossible_coordinates;
            detail::Stop* prev_stop = nullptr;
            std::for_each(bus.stops.begin(), bus.stops.end(), [this, &length, &length_geo, &temp_coords, &prev_stop, impossible_coordinates] (auto this_stop) {
                if(temp_coords == impossible_coordinates && prev_stop == nullptr) {
                    temp_coords = this_stop->location;
                    prev_stop = this_stop;
                } else {
                    length += GetStopDistance(prev_stop, this_stop);
                    length_geo += geo::ComputeDistance(temp_coords, this_stop->location);
                    temp_coords = this_stop->location;
                    prev_stop = this_stop;
                }
            });
            double curvature = length / length_geo;

            std::vector<detail::Stop*> temp(bus.stops);
            std::sort(temp.begin(), temp.end());
            int unique_stop_count = std::distance(temp.begin(), std::unique(temp.begin(), temp.end()));
            return {name, stop_count, unique_stop_count, length, curvature};
        }

        detail::StopInfo TransportCatalogue::GetStopInfo(detail::Stop* stop) {
            detail::StopInfo info;
            info.name = stop->name;
            std::for_each(stop->buses.begin(), stop->buses.end(), [&info](auto bus) {
                info.buses.insert(bus->name);
            });
            return info;
        }

        std::vector<detail::Bus*> TransportCatalogue::GetAllBuses() {
            std::vector<detail::Bus*> result;
            for(detail::Bus& bus : buses_) {
                result.push_back(&bus);
            }
            std::sort(result.begin(), result.end(), [](detail::Bus* lhs, detail::Bus* rhs){
                return lhs->name < rhs->name;
            });
            return result;
        }

        std::vector<detail::Stop*> TransportCatalogue::GetAllStops() {
            std::vector<detail::Stop*> result;
            for(detail::Stop& stop : stops_) {
                if(stop.buses.size() != 0) {
                    result.push_back(&stop);
                }
            }
            std::sort(result.begin(), result.end(), [](detail::Stop* lhs, detail::Stop* rhs){
                return lhs->name < rhs->name;
            });
            return result;
        }

        std::vector<detail::Stop*> TransportCatalogue::GetAllStops(bool to_proto) {
            std::vector<detail::Stop*> result;
            for(detail::Stop& stop : stops_) {
                    result.push_back(&stop);
            }
            return result;
        }

         std::unordered_map<detail::Stop*, std::unordered_map<detail::Stop*, int>> TransportCatalogue::GetAllDistances() const {
            return distances_;
        }

        //private методы-хелперы для построения RouteGraphData
         void TransportCatalogue::GetVertexIds() {
             //непонятно почему разные результаты при переборе stops_ и stopname_to_stop_
             for(auto& stop : stops_) {
                 //добавляем в поле класса для передачи в класс, который будет искать кратчайший путь
                 if(stopname_to_vertices_.count(stop.name) == 0) {
                     stopname_to_vertices_[stop.name] = {&stop,  vertice_counter_, vertice_counter_ + 1};
                     vertice_counter_ += 2;
                 }
             }

         }

         std::vector<graph::Edge<detail::Weight>> TransportCatalogue::GetBusEdges(const detail::Bus* bus, const detail::RoutingSettings& settings) {
                     std::vector<graph::Edge<detail::Weight>> result;
                     for(int i = 0; i < bus->stops.size(); ++i) {
                         //вводим временные меременные чтобы не писать 3-строчные аргументы
                         auto name = bus->stops[i]->name;
                         auto i_value = stopname_to_vertices_.at(name);
                         result.push_back({i_value.wait, i_value.bus, {static_cast<double>(settings.bus_wait_time), true, 0, i_value.stop->name}});
                         ++edge_counter_;
                         int distance = 0;
                         for(int j = i + 1; j < bus->stops.size(); ++j) {
                             auto j_value = stopname_to_vertices_.at(bus->stops[j]->name);
                             auto prev_to_j_value = stopname_to_vertices_.at(bus->stops[j - 1]->name);
                             if(distances_.at(prev_to_j_value.stop).count(prev_to_j_value.stop) != 0) {
                                 distance += distances_.at(prev_to_j_value.stop).at(prev_to_j_value.stop);
                             }
                             distance += distances_.at(prev_to_j_value.stop).at(j_value.stop);
                             result.push_back({i_value.bus, j_value.wait, {distance / settings.bus_velocity, false, j - i, bus->name}});
                             ++edge_counter_;
                         }
                     }
                     if(!bus->is_looped) {
                         for(int i = bus->stops.size() - 1; i > 0; i--) {
                             int distance = 0;
                             auto name = bus->stops[i]->name;
                             auto i_value = stopname_to_vertices_.at(name);
                             for(int j = i - 1; j >= 0; j--) {
                                 auto j_value = stopname_to_vertices_.at(bus->stops[j]->name);
                                 auto next_to_j_value = stopname_to_vertices_.at(bus->stops[j + 1]->name);
                                 if(distances_.at(next_to_j_value.stop).count(next_to_j_value.stop) != 0) {
                                     distance += distances_.at(next_to_j_value.stop).at(next_to_j_value.stop);
                                 }
                                 distance += distances_.at(next_to_j_value.stop).at(j_value.stop);
                                 result.push_back({i_value.bus, j_value.wait, {distance / settings.bus_velocity, false, i - j, bus->name}});
                                 ++edge_counter_;
                             }
                         }
                     }

                     return result;
                 }

        //Построение RouteGraphData для всех маршрутов для построения графа
        std::vector<graph::Edge<detail::Weight>> TransportCatalogue::GetGraphData(const detail::RoutingSettings& settings) {
            if(stopname_to_vertices_.size() == 0) {
                GetVertexIds();
            }
            std::vector<graph::Edge<detail::Weight>> result;
            for(const auto& bus : buses_) {
                std::vector<graph::Edge<detail::Weight>> edges = GetBusEdges(&bus, settings);
                //перемещаем рёбра в выходной результат{
                result.insert(result.end(), std::make_move_iterator(edges.begin()), std::make_move_iterator(edges.end()));
            }
            return result;
        }

        std::optional<graph::VertexId> TransportCatalogue::GetStopVertex(std::string_view name) const {
                    if(stopname_to_vertices_.count(name) != 0) {
                        return stopname_to_vertices_.at(name).wait;
                    } else {
                        return {};
                    }
                }

        size_t TransportCatalogue::GetVertexCount() const {
            return vertice_counter_;
        }

        std::unordered_map<detail::Stop*, detail::StopVertices> TransportCatalogue::GetVertices() {
            std::unordered_map< detail::Stop*, detail::StopVertices> result;
            for(auto [name, vertices] : stopname_to_vertices_) {
                result[GetStop(name)] = vertices;
            }
            return result;
        }
        void TransportCatalogue::SetVertices(std::unordered_map<std::string_view, detail::StopVertices> vertices) {
            stopname_to_vertices_ = vertices;
    }
}
