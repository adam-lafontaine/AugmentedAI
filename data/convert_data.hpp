#pragma once
#include "../utils/libimage.hpp"

#include <vector>
#include <string>

namespace img = libimage;

namespace convert_data
{
	using src_data_t = std::vector<double>;
	using data_list_t = std::vector<src_data_t>;
	using path_t = std::string;
	using file_list_t = std::vector<path_t>;

	void convert_and_save(data_list_t const& data, const char* dst_dir);

	src_data_t read_converted(img::view_t const& row_view);
}