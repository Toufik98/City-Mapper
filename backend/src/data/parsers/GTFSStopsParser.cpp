#include "GTFSStopsParser.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace city_mapper {

static void replaceAll(std::string &str, const std::string &from,
                       const std::string &to) {
  if (from.empty())
    return;
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
}

std::string GTFSStopsParser::normalizeName(const std::string &name) {
  std::string res = name;

  // Transform to lowercase (handles simple ASCII)
  std::transform(res.begin(), res.end(), res.begin(), ::tolower);

  // Handle some common French UTF-8 lowercase chars
  replaceAll(res, "é", "e");
  replaceAll(res, "è", "e");
  replaceAll(res, "ê", "e");
  replaceAll(res, "ë", "e");
  replaceAll(res, "à", "a");
  replaceAll(res, "â", "a");
  replaceAll(res, "ä", "a");
  replaceAll(res, "î", "i");
  replaceAll(res, "ï", "i");
  replaceAll(res, "ô", "o");
  replaceAll(res, "ö", "o");
  replaceAll(res, "ù", "u");
  replaceAll(res, "û", "u");
  replaceAll(res, "ü", "u");
  replaceAll(res, "ç", "c");
  replaceAll(res, "œ", "oe");

  // Also replace uppercase UTF-8 just in case std::transform didn't catch them
  replaceAll(res, "É", "e");
  replaceAll(res, "È", "e");
  replaceAll(res, "Ê", "e");
  replaceAll(res, "Ë", "e");
  replaceAll(res, "À", "a");
  replaceAll(res, "Â", "a");
  replaceAll(res, "Ä", "a");
  replaceAll(res, "Î", "i");
  replaceAll(res, "Ï", "i");
  replaceAll(res, "Ô", "o");
  replaceAll(res, "Ö", "o");
  replaceAll(res, "Ù", "u");
  replaceAll(res, "Û", "u");
  replaceAll(res, "Ü", "u");
  replaceAll(res, "Ç", "c");
  replaceAll(res, "Œ", "oe");

  // Remove dashes and special chars to improve matching
  replaceAll(res, "-", " ");
  replaceAll(res, "'", " ");
  replaceAll(res, "(", "");
  replaceAll(res, ")", "");
  replaceAll(res, ".", "");

  // Trim extra spaces
  std::string trimmed;
  bool in_space = false;
  for (char c : res) {
    if (c == ' ') {
      if (!in_space && !trimmed.empty()) {
        trimmed += c;
        in_space = true;
      }
    } else {
      trimmed += c;
      in_space = false;
    }
  }
  if (!trimmed.empty() && trimmed.back() == ' ') {
    trimmed.pop_back();
  }

  return trimmed;
}

std::vector<std::string> splitCSVLine(const std::string &line) {
  std::vector<std::string> result;
  std::string current;
  bool in_quotes = false;

  for (size_t i = 0; i < line.length(); ++i) {
    char c = line[i];
    if (c == '"') {
      in_quotes = !in_quotes;
    } else if (c == ',' && !in_quotes) {
      result.push_back(current);
      current.clear();
    } else {
      current += c;
    }
  }
  result.push_back(current);

  return result;
}

std::unordered_map<std::string, std::pair<double, double>>
GTFSStopsParser::parse(const std::string &filename) const {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("GTFSStopsParser: Cannot open file: " + filename);
  }

  std::unordered_map<std::string, std::pair<double, double>> stops;
  std::string line;

  // Skip header line
  std::getline(file, line);

  // Columns typically:
  // stop_id,stop_code,stop_name,stop_desc,stop_lon,stop_lat... Let's find
  // column indices dynamically just in case
  int name_idx = -1;
  int lat_idx = -1;
  int lon_idx = -1;

  auto headers = splitCSVLine(line);
  for (size_t i = 0; i < headers.size(); ++i) {
    // Strip out any trailing/leading carriage returns or quotes
    std::string header = headers[i];
    while (!header.empty() && (header.back() == '\r' || header.back() == '\n'))
      header.pop_back();

    if (header == "stop_name")
      name_idx = i;
    else if (header == "stop_lat")
      lat_idx = i;
    else if (header == "stop_lon")
      lon_idx = i;
  }

  if (name_idx == -1 || lat_idx == -1 || lon_idx == -1) {
    throw std::runtime_error(
        "GTFSStopsParser: Missing required columns in header.");
  }

  while (std::getline(file, line)) {
    if (line.empty())
      continue;

    auto tokens = splitCSVLine(line);
    if (tokens.size() <= std::max({name_idx, lat_idx, lon_idx}))
      continue;

    std::string raw_name = tokens[name_idx];
    std::string lat_str = tokens[lat_idx];
    std::string lon_str = tokens[lon_idx];

    try {
      double lat = std::stod(lat_str);
      double lon = std::stod(lon_str);

      std::string norm_name = normalizeName(raw_name);
      stops[norm_name] = {lat, lon};
    } catch (const std::exception &) {
      // Ignore parse errors for latitude/longitude
    }
  }

  return stops;
}

} // namespace city_mapper
