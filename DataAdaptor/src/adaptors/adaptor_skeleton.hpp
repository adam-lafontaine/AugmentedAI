#pragma once

#include "../data_adaptor.hpp"

#include <cassert>

#ifdef __linux

#define sprintf_s sprintf

#endif


using feature_pixel_t = data_adaptor::feature_pixel_t;
using features_t = data_adaptor::features_t;


constexpr size_t MAX_FEATURE_IMAGE_SIZE = 10000;
constexpr size_t FEATURE_IMAGE_WIDTH = 1;
constexpr r64 FEATURE_MIN_VALUE = 0;
constexpr r64 FEATURE_MAX_VALUE = 1;


namespace impl
{
	// Define how to name save files
	inline std::string make_numbered_file_name(u32 index, size_t index_length)
	{
		index_length = index_length < 2 ? 2 : index_length;

		char idx_str[10];
		sprintf_s(idx_str, "%0*d", (int)index_length, index); // zero pad index number

		return std::string(idx_str) + data_adaptor::FEATURE_IMAGE_EXTENSION;
	}


	//======= TODO: IMPLEMENT =================


	// How a file is read and converted to numeric data
	inline features_t file_to_features(const char* src_file)
	{
		features_t data{ 0 };

		assert(data.size() == FEATURE_IMAGE_WIDTH);

		return data;
	}


	// How each value of data is to be represented as a pixel
	inline feature_pixel_t value_to_feature_pixel(r64 val)
	{
		assert(val >= FEATURE_MIN_VALUE);
		assert(val <= FEATURE_MAX_VALUE);

		return 0;
	}


	// How each data pixel is to be converted back to numeric data
	inline r64 feature_pixel_to_value(feature_pixel_t const& pix)
	{
		return 0;
	}


	
}