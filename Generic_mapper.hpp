#pragma once

#include <vector>
#include <utility>

#include "Generic_connection_parser.hpp"

namespace travel{
  class Generic_mapper: public Generic_connection_parser{
  public:
    virtual std::vector<std::pair<uint64_t,uint64_t> > compute_travel(uint64_t _start, uint64_t _end) = 0;
    virtual std::vector<std::pair<uint64_t,uint64_t> > compute_and_display_travel(uint64_t _start, uint64_t _end) = 0;

    virtual std::vector<std::pair<uint64_t,uint64_t> > compute_travel(const std::string&, const std::string&){
      throw("Nothing here");
    }
    virtual std::vector<std::pair<uint64_t,uint64_t> > compute_and_display_travel(const std::string&, const std::string&){
      throw("Nothing here");
    }
  };
}
