#pragma once
/*Inclusion du fichier generic pour le parse des fichiers*/
#include "Generic_station_parser.hpp"
#include "Generic_connection_parser.hpp"
#include "Generic_mapper.hpp"
/*Inclusion de la biblioteque stream*/
#include <fstream>

/*Implementation de classe Station_parser*/
class Station_parser: public travel::Generic_mapper{
  public :
    void set_stations(const std::string & _filename);
    void set_connections(const std::string& _filename);
    std::vector<std::pair<uint64_t,uint64_t> > compute_travel(uint64_t _start, uint64_t _end) override;
    std::vector<std::pair<uint64_t,uint64_t> > compute_and_display_travel(uint64_t _start, uint64_t _end);
    uint64_t choose_station_id(const std::string& _type); 

  protected:
    void read_stations(const std::string & _filename) override;
    void read_connections(const std::string& _filename) override;

};

//Fonctions auxiliaire utiles 
 
void sec_to_hhmmss(const uint64_t temps_sec);
bool compare_char(const char& c1, const char& c2);
bool compare_string(const std::string& s1, const std::string& s2);
bool sort_by_cost(const std::pair<uint64_t, uint64_t>& p1, const std::pair<uint64_t, uint64_t>&p2);
uint64_t valid_int_input();