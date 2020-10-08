#pragma once

#include "../data_adaptor.hpp"

#ifdef __linux

#define sprintf_s sprintf

#endif


using data_pixel_t = data_adaptor::data_pixel_t;
using src_data_t = data_adaptor::src_data_t;


constexpr size_t MAX_DATA_IMAGE_SIZE = 10000;
constexpr size_t DATA_IMAGE_WIDTH = 1;
constexpr double DATA_MIN_VALUE = 0;
constexpr double DATA_MAX_VALUE = 1;






namespace impl
{
	// Define how to name save files
	inline std::string make_numbered_file_name(unsigned index, size_t index_length)
	{
		index_length = index_length < 2 ? 2 : index_length;

		char idx_str[10];
		sprintf_s(idx_str, "%0*d", (int)index_length, index); // zero pad index number

		return std::string(idx_str) + img::IMAGE_FILE_EXTENSION;
	}


	//======= TODO: IMPLEMENT =================

	inline data_pixel_t data_value_to_data_pixel(double val)
	{
		assert(val >= DATA_MIN_VALUE);
		assert(val <= DATA_MAX_VALUE);

		return img::to_pixel(255, 255, 255, 255);
	}


	inline double data_pixel_to_data_value(data_pixel_t const& pix)
	{
		return 0;
	}


	inline src_data_t file_to_data(const char* src_file)
	{
		src_data_t data{ 0 };

		assert(data.size() == DATA_IMAGE_WIDTH);

		return data;
	}
}