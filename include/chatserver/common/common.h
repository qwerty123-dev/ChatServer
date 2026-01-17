#pragma once
#include <string>
#include <unordered_map>

namespace chatserver::common {

std::unordered_map<std::string, std::string> parse_ini(const std::string& filename);

}

