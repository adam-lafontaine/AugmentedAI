#pragma once

#include "../../utils/libimage.hpp"

#include <string>
#include <vector>

namespace img = libimage;

// Implementations of these functions will be specific for each application.
namespace data_adaptor
{
	using src_data_t = std::vector<double>;
	using data_list_t = std::vector<src_data_t>;
	using path_t = std::string;
	using file_list_t = std::vector<path_t>;
	using data_pixel_t = img::pixel_t;

	// Define how files are interpreted as data to be processed.
	src_data_t file_to_data(const char* src_file);
	src_data_t file_to_data(path_t const& src_file);

	data_list_t file_list_to_data(file_list_t const& files);

	data_list_t file_list_to_data_par(file_list_t const& files);  // experimental

	// Define how data is to be saved as a "data image"
	void save_data_images(data_list_t const& data, const char* dst_dir);
	void save_data_images(data_list_t const& data, path_t const& dst_dir);

	// TODO: files_to_data_images()

	// Define how values are converted to pixels and vice versa
	data_pixel_t data_value_to_data_pixel(double val);   // TODO: tests
	double data_pixel_to_data_value(data_pixel_t const& pix);

	// Define how one row of a "data image" is converted back to data
	src_data_t data_image_row_to_data(img::view_t const& pixel_row);

	// make data properties public
	size_t data_image_width();
	double data_min_value();
	double data_max_value();


	
}
