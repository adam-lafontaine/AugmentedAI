#pragma once
/*

Copyright (c) 2021 Adam Lafontaine

*/

#include <vector>
#include <string>
#include <unordered_map>

namespace config_reader
{
    using config_t = std::unordered_map<std::string, std::string>;
    using key_list_t = std::vector<const char*>;

    config_t read_config(const char* file_path);

    config_t read_config(const char* file_path, key_list_t const& required_keys);
}