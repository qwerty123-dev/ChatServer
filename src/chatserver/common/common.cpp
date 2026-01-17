#include "chatserver/common/common.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace chatserver::common {

std::unordered_map<std::string, std::string> parse_ini(const std::string& filename) {
    std::unordered_map<std::string, std::string> result;
    std::ifstream file(filename);
    if(!file.is_open()) return result;

    std::string line;
    while(std::getline(file, line)) {
        // убираем пробелы
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

        if(line.empty() || line[0] == '#' || line[0] == ';') continue;
        if(line[0] == '[') continue; // секции

        auto pos = line.find('=');
        if(pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        result[key] = value;
    }
    return result;
}

}

