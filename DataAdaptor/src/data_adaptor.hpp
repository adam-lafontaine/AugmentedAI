#pragma once

#include "../../utils/libimage.hpp"

#include <string>
#include <vector>

namespace img = libimage;

/*

Preprocess file data and save "data images" to be processed by the algorithm at a later time.
Implementations of some of these functions will be specific for each application.
Each row of pixels in a data image coresponds to a row of data (i.e. one file)

This particular project uses png files as the source data and a K-means clustering derivative to classify the data.
Any file type and algorithm can used.
The idea is to be able to "compress" large amounts of files for later use with several algorithms.
The data images are png files with 4 8bit channel pixels.  The channels/bits can be used to store data however you like.

*/
namespace data_adaptor
{
	using src_data_t = std::vector<double>;      // source data converted from a file
	using data_list_t = std::vector<src_data_t>; // data from multiple files
	using path_t = std::string;                  // file path.  Change to std::filesystem::path?
	using file_list_t = std::vector<path_t>;     // list of files
	using data_pixel_t = img::pixel_t;           // Once data value converted to a pixel (4 x 8bit)

	/*

	These functions require custom implementations in the impl namespace.
	...DataAdaptor/src/adaptors/ contains a skeleton and some examples.
	Include the implementations in data_adaptor.cpp

	*/

	
	// Define how an individual file is interpreted as data to be processed.
	// The file type and how it is processed depends on the implementation.
	// For the application, decide how a file produces a vector of doubles	
	src_data_t file_to_data(const char* src_file);
	src_data_t file_to_data(path_t const& src_file);


	// Define how values are converted to pixels and vice versa
	data_pixel_t data_value_to_data_pixel(double val);   // TODO: tests
	double data_pixel_to_data_value(data_pixel_t const& pix);


	// Make data properties public
	// Constants DATA_IMAGE_WIDTH, DATA_MIN_VALUE, and DATA_MAX_VALUE must be defined in the impl namespace
	size_t data_image_width();
	double data_min_value();
	double data_max_value();


	/*

	These functions are implemented in data_adaptor.cpp using the above functions

	*/


	// Convert files to data to be processed
	data_list_t file_list_to_data(file_list_t const& files);


	// Save source data as a "data image"
	void save_data_images(data_list_t const& data, const char* dst_dir);
	void save_data_images(data_list_t const& data, path_t const& dst_dir);


	// Convert one row of a "data image" back to source data
	src_data_t data_image_row_to_data(img::view_t const& pixel_row);
	
}
