#include<iostream>
#include "station_parser.h"
#include "Grade.hpp"

int main(){
    Station_parser csv_station_parser;
    csv_station_parser.set_stations("data/s.csv");
    csv_station_parser.set_connections("data/c.csv");

    travel::Grade grader(false);

    grader.stations(csv_station_parser);
    grader.connections(csv_station_parser);

    /**
    std::unordered_map<uint64_t, travel::Station> h; 
    h = csv_station_parser.get_stations_hashmap();
    for(auto it : h){
        std::cout<< "ID : "<< it.first << ", " << it.second.name <<" , " <<it.second.address << ", "<< it.second.line_name << std::endl;
    }
    */

    return 0;
}