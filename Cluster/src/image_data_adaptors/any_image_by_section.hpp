#pragma once

#include "../../../DataAdaptor/src/data_adaptor.hpp"

#include <boost/gil/extension/io/bmp.hpp>

namespace gil = boost::gil;

#ifdef __linux

#define sprintf_s sprintf

#endif


using data_pixel_t = data_adaptor::data_pixel_t;
using src_data_t = data_adaptor::src_data_t;


//======= DATA PROPERTIES =================

constexpr size_t NUM_GRAY_SHADES = 256;
constexpr size_t MAX_DATA_IMAGE_SIZE = 500 * 500;

constexpr size_t DATA_IMAGE_WIDTH = 1;// NUM_GRAY_SHADES;
constexpr double DATA_MIN_VALUE = 0;
constexpr double DATA_MAX_VALUE = 1;

constexpr auto BITS32_MAX = img::to_bits32(255, 255, 255, 255);


// for getting bytes from 32 bit values
union four_bytes_t
{
	img::bits32 value;
	img::bits8 bytes[4];
};






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

		const auto ratio = (val - DATA_MIN_VALUE) / (DATA_MAX_VALUE - DATA_MIN_VALUE);

		four_bytes_t x;
		x.value = static_cast<img::bits32>(ratio * BITS32_MAX);

		const auto r = x.bytes[3];
		const auto g = x.bytes[2];
		const auto b = x.bytes[1];
		const auto a = x.bytes[0];

		return img::to_pixel(r, g, b, a);
	}


	inline double data_pixel_to_data_value(data_pixel_t const& pix)
	{
		const auto rgba = img::to_rgba(pix);

		four_bytes_t x;

		x.bytes[3] = rgba.r;
		x.bytes[2] = rgba.g;
		x.bytes[1] = rgba.b;
		x.bytes[0] = rgba.a;

		return static_cast<double>(x.value) / BITS32_MAX;
	}


	inline src_data_t file_to_data(const char* src_file)
	{

		gil::rgb8_image_t image;
		gil::read_and_convert_image(src_file, image, gil::bmp_tag());
		

		src_data_t data{ 0 };

		assert(data.size() == DATA_IMAGE_WIDTH);

		return data;
	}
}