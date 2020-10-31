#include "config_reader.hpp"

#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

using istream_t = std::fstream;

constexpr auto DELIM = '=';
constexpr auto COMMENT = '#';

namespace config_reader
{
    static void trim(std::string& str)
    {
        str.erase(0, str.find_first_not_of(' '));

        str.erase(str.find_last_not_of(' ') + 1);
    }


    static config_t make_config(const char* file_path)
    {
        config_t config_map;

        if (!fs::exists(file_path))
        {
            config_map["error"] = std::string(file_path) + " does not exist";
            return config_map;
        }

        if (!fs::is_regular_file(file_path))
        {
            config_map["error"] = std::string(file_path) + " is not a file";
            return config_map;
        }

        return config_map;
    }


    config_t read_config(const char* file_path)
    {
        auto config_map = make_config(file_path);
        if (!config_map.empty())
            return config_map;

        istream_t file_in;
        file_in.open(file_path);    

        std::string line;
        while (std::getline(file_in, line))
        {
            std::istringstream line_in(line);
            std::string key;

            if (!std::getline(line_in, key, DELIM) // lines without '=' deliminator are ignored
                || key[0] == COMMENT)              // lines beginning with '#' are ignored    
                continue;

            // lines without a value after the '=' are ignored
            std::string value;
            if (!std::getline(line_in, value))
                continue;

            trim(key);
            trim(value);

            config_map.insert_or_assign(std::move(key), std::move(value));
        }

        file_in.close();

        return config_map;
    }


    config_t read_config(const char* file_path, key_list_t const& required_keys)
    {
        auto config_map = make_config(file_path);
        if (!config_map.empty())
            return config_map;

        istream_t file_in;
        file_in.open(file_path);

        const auto contains = [&](std::string const& key) 
        {
            const auto b = required_keys.begin();
            const auto e = required_keys.end();
            return std::any_of(b, e, [&](const char* s) { return key.compare(s) == 0; });
        };

        std::string line;
        while (std::getline(file_in, line))
        {
            std::istringstream line_in(line);
            std::string key;        
            
            if (!std::getline(line_in, key, DELIM) // lines without '=' deliminator are ignored
                || key[0] == COMMENT)              // lines beginning with '#' are ignored    
                continue;

            trim(key);

            if (!contains(key)) // keys that are not required are ignored
                continue;

            // lines without a value after the '=' are ignored
            std::string value;
            if (!std::getline(line_in, value))
                continue;
            
            trim(value);

            config_map.insert_or_assign(std::move(key), std::move(value));
        }

        file_in.close();

        return config_map;

        // all keys are in the map
        //if (config_map.size() == required_keys.size())
        //    return config_map;

        //// one or more keys are missing
        //config_t error_map;

        //for (auto const& key : required_keys)
        //{
        //    if (config_map.find(key) == config_map.end())
        //        error_map.insert_or_assign(std::string("error_") + key, "missing key");
        //}

        //return error_map;
    }
}