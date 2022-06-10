#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <cstdint>

namespace fs = std::filesystem;

using r64 = double;
using u32 = uint32_t;


/*

Preprocess file data and save "feature images" to be processed by the algorithm at a later time.
Implementations of some of these functions will be specific for each application.
Each row of pixels in a data image corresponds to a feature vector of one file

This particular project uses png files as the source data and a K-means clustering derivative to classify the data.
Any file type and algorithm can used.
The idea is to be able to "compress" large amounts of files for later use with several algorithms.
The data images are png files with 4 8bit channel pixels.  The channels/bits can be used to store data however you like.
Binary files could be used instead but images allow users to see what their data "looks like"

*/
namespace data_adaptor
{
	using src_data_t = std::vector<r64>;        // feature vector from the file
	using data_list_t = std::vector<src_data_t>;   // multiple feature vectors
	using path_t = fs::path;                       // file path.
	using file_list_t = std::vector<path_t>;       // list of files
	using feature_pixel_t = u32;                 // One feature value converted to a pixel (4 x 8bit)
	using pixel_row_t = std::vector<feature_pixel_t>; // A single row of data pixels

	constexpr auto FEATURE_IMAGE_EXTENSION = ".png";

	/*

	These functions require custom implementations in the impl namespace.
	...DataAdaptor/src/adaptors/ contains a skeleton and some examples.
	Include the implementations in data_adaptor.cpp

	*/

	
	// Define how an individual file is interpreted as data to be processed.
	// The file type and how it is processed depends on the implementation.
	// For the application, decide how a file produces a vector of r64s	
	src_data_t file_to_data(const char* src_file);
	src_data_t file_to_data(path_t const& src_file);


	// Define how values are converted to pixels and vice versa
	feature_pixel_t value_to_feature_pixel(r64 val);   // TODO: tests
	r64 feature_pixel_to_value(feature_pixel_t const& pix);


	// Make data properties public
	// Constants FEATURE_IMAGE_WIDTH, FEATURE_MIN_VALUE, and FEATURE_MAX_VALUE must be defined in the impl namespace
	size_t feature_image_width();
	r64 feature_min_value();
	r64 feature_max_value();


	/*

	These functions are implemented in data_adaptor.cpp using the above functions

	*/


	// Convert files to data to be processed
	data_list_t file_list_to_data(file_list_t const& files);


	// Save source data as a "data image"
	void save_feature_images(data_list_t const& data, const char* dst_dir);
	void save_feature_images(data_list_t const& data, path_t const& dst_dir);


	// Convert one row of a "data image" back to source data
	src_data_t feature_image_row_to_data(pixel_row_t const& pixel_row);
	
}
