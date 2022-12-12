#ifndef GENERIC_STATION_PARSER_H
#define GENERIC_STATION_PARSER_H

#pragma once

#include <string>
#include <iostream>
#include <unordered_map>

namespace travel{
  struct Station{
    std::string name;
    std::string line_id;
    std::string address;
    std::string line_name;
    friend std::ostream& operator<<(std::ostream& _os, const Station& _stop){
      _os << "Station: " << _stop.name << " (line " << _stop.line_id << ")";
      return _os;
    }
  };

  class Generic_station_parser{
  private:
    // Unordered map to store the stations : key is the station id and value is the station
    const std::unordered_map<uint64_t, Station>& get_stations_hashmap() const{
      return this->stations_hashmap;
    }

  protected:
    virtual void read_stations(const std::string& _filename) = 0;

  protected:
    std::unordered_map<uint64_t, Station> stations_hashmap;

  public:
    // Getter for stations hashmap
    const std::unordered_map<uint64_t, Station> &get_stations() const
    {
      return this->get_stations_hashmap();
    };
}

#endif
