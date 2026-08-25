#pragma once
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../util/log.hpp"

class ini {
    private:
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> conf;
    std::vector<std::string> section_keys = {"", "bar"};

    public:
    static ini parse(std::string);
    std::unordered_map<std::string, std::string> &operator[](std::string key) {
        if (!(std::find(section_keys.begin(), section_keys.end(), key) != section_keys.end())) {
            lg::err(std::format("invalid section: '{}'", key));
            std::exit(1);
        }
        return conf[key];
    }
    bool contains(const std::string &section) {
        return conf.find(section) != conf.end();
    }
    bool contains(const std::string &section, const std::string &key) {
        auto it = conf.find(section);
        if (it == conf.end())
            return false;
        return it->second.find(key) != it->second.end();
    }
};
