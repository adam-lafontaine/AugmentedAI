#pragma once

#include "../data_adaptor.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <cstdint>

#ifdef __linux

#define sprintf_s sprintf

#endif


using data_pixel_t = data_adaptor::data_pixel_t;
using src_data_t = data_adaptor::src_data_t;


//======= DATA PROPERTIES =================

constexpr size_t NUM_GRAY_SHADES = 256;
constexpr size_t MAX_DATA_IMAGE_SIZE = 500 * 500;

constexpr size_t DATA_IMAGE_WIDTH = NUM_GRAY_SHADES;
constexpr double DATA_MIN_VALUE = 0;
constexpr double DATA_MAX_VALUE = 1;

constexpr auto BITS32_MAX = img::to_bits32(255, 255, 255, 255);


// for getting bytes from 32 bit values
union four_bytes_t
{
	img::bits32 value;
	img::bits8 bytes[4];
};


// Define how to name save files
inline std::string make_numbered_file_name(unsigned index, size_t index_length)
{
	index_length = index_length < 2 ? 2 : index_length;

	char idx_str[10];
	sprintf_s(idx_str, "%0*d", (int)index_length, index); // zero pad index number

	std::time_t result = std::time(nullptr);

	std::ostringstream oss;
	oss << std::put_time(std::localtime(&result), "%F_%T");

	auto date_file = oss.str() + img::IMAGE_FILE_EXTENSION;

	std::replace(date_file.begin(), date_file.end(), ':', '-');

	return std::string(idx_str) + '_' + date_file;
}



//======= HELPERS =================


// counts the amount of each shade found in the image
// returns a histogram of relative amounts from 0 - 1
inline src_data_t count_shades(img::gray::view_t const& view)
{
	const auto hist = img::gray::make_histogram(view);

	src_data_t data(hist.size(), 0);

	const auto total = static_cast<double>(view.width() * view.height());

	for (size_t i = 0; i < hist.size(); ++i)
	{
		data[i] = static_cast<double>(hist[i]) / total;
	}

	return data;
}


namespace impl
{
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
		auto image = img::gray::read_image_from_file(src_file);
		const auto data = count_shades(img::make_view(image));

		assert(data.size() == DATA_IMAGE_WIDTH);

		return data;
	}
}