#pragma once

#include <string>
#include <vector>

namespace read_data
{
	using src_data_t = std::vector<double>;
	using data_list_t = std::vector<src_data_t>;
	using path_t = std::string;
	using file_list_t = std::vector<path_t>;

	src_data_t read_source_file(const char* src_file);
	src_data_t read_source_file(path_t const& src_file);

	data_list_t read_source_files(file_list_t const& files);
}
