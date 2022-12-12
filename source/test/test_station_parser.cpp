/*
Author : 	Toufik FERHAT
Date :		2022-12-12
Version :	1.0
Description :	Unit tests for the station parser
*/

// Include gtest 
#include "gtest/gtest.h"
#include "gmock/gmock.h"

// Include the tested class
#include "station_parser.hpp"



// Test the station parser
TEST(StationParser, test_read_station){
    // Create a station parser
    StationParser station_parser;
    // Read the station file
    station_parser.read_stations("data/stations.csv");
    // Get the stations
    std::vector<Station> stations = station_parser.get_stations();
    // Check the number of stations
}










