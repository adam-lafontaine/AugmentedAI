#pragma once

#include "../data_adaptor.hpp"
#include "../../../utils/libimage/libimage.hpp"

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

namespace img = libimage;


using data_pixel_t = data_adaptor::data_pixel_t;
using src_data_t = data_adaptor::src_data_t;
using path_t = data_adaptor::path_t;


//======= DATA PROPERTIES =================

constexpr size_t NUM_GRAY_SHADES = 256;
constexpr size_t MAX_DATA_IMAGE_SIZE = 500 * 500;


constexpr auto BITS32_MAX = UINT32_MAX;


//======= HELPERS =================


// counts the amount of each shade found in the image
// returns a histogram of relative amounts from 0 - 1
static src_data_t count_shades(img::gray::view_t const& view)
{
	const auto hist = img::calc_stats(view).hist;

	src_data_t data(hist.size(), 0);

	const auto total = static_cast<r64>(view.width) * view.height;

	for (size_t i = 0; i < hist.size(); ++i)
	{
		data[i] = static_cast<r64>(hist[i]) / total;
	}

	return data;
}


namespace impl
{
	constexpr size_t DATA_IMAGE_WIDTH = NUM_GRAY_SHADES;
	constexpr r64 DATA_MIN_VALUE = 0;
	constexpr r64 DATA_MAX_VALUE = 1;



	// Define how to name save files
	inline std::string make_numbered_file_name(u32 index, size_t index_length)
	{
		index_length = index_length < 2 ? 2 : index_length;

		char idx_str[10];
		sprintf_s(idx_str, "%0*d", (int)index_length, index); // zero pad index number

		return std::string(idx_str) + data_adaptor::DATA_IMAGE_EXTENSION;
	}


	inline data_pixel_t data_value_to_data_pixel(r64 val)
	{
		assert(val >= DATA_MIN_VALUE);
		assert(val <= DATA_MAX_VALUE);

		const auto ratio = (val - DATA_MIN_VALUE) / (DATA_MAX_VALUE - DATA_MIN_VALUE);

		img::pixel_t color{};
		color.value = static_cast<u32>(ratio * BITS32_MAX);

		return color.value;
	}


	inline r64 data_pixel_to_data_value(data_pixel_t const& pix)
	{
		return static_cast<r64>(pix) / BITS32_MAX;
	}


	inline src_data_t file_to_data(const char* src_file)
	{
		img::gray::image_t image;
		
		img::read_image_from_file(src_file, image);
		const auto data = count_shades(img::make_view(image));

		assert(data.size() == DATA_IMAGE_WIDTH);

		return data;
	}
}