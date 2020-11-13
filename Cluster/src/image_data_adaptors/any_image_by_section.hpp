#pragma once

#include "../../../DataAdaptor/src/data_adaptor.hpp"

#include <boost/gil/extension/io/bmp.hpp>

#include <array>
#include <vector>
#include <algorithm>

namespace gil = boost::gil;

#ifdef __linux

#define sprintf_s sprintf

#endif


using data_pixel_t = data_adaptor::data_pixel_t;
using src_data_t = data_adaptor::src_data_t;

using src_image_t = img::gray::image_t;
using src_view_t = img::gray::view_t;
using src_pixel_t = img::gray::pixel_t;


// for getting bytes from 32 bit values
union four_bytes_t
{
	img::bits32 value;
	img::bits8 bytes[4];
};


typedef struct
{
	img::bits8 min;
	img::bits8 max;

} bits8_range_t;

//======= DATA PROPERTIES =================

constexpr size_t SRC_IMAGE_WIDTH = 2448;
constexpr size_t SRC_IMAGE_HEIGHT = 2048;
constexpr size_t RANGE_COUNT = 2;

constexpr img::bits8 MAX_SHADE = 255;
constexpr size_t NUM_GRAY_SHADES = 256;
constexpr size_t MAX_DATA_IMAGE_SIZE = 500 * 500;

constexpr size_t DATA_IMAGE_WIDTH = 400;// NUM_GRAY_SHADES;
constexpr double DATA_MIN_VALUE = 0;
constexpr double DATA_MAX_VALUE = 1;

constexpr auto BITS32_MAX = img::to_bits32(255, 255, 255, 255);
constexpr auto BITS24_MAX = img::to_bits32(255, 255, 255);
constexpr auto BITS16_MAX = UINT16_MAX;

constexpr std::array<img::pixel_range_t, RANGE_COUNT> pixel_ranges = 
{
	{{150, 480, 500, 1600}, {1730, 2060, 500, 1600}}
};

using view_list_t = std::vector<src_view_t>;




//======= HELPERS ==============

src_view_t make_subimage_view(src_view_t& view, img::pixel_range_t const& range)
{
	auto const width = range.x_end - range.x_begin;
	auto const height = range.y_end - range.y_begin;
	
	return gil::subimage_view(view, range.x_begin, range.y_begin, width, height);
}


view_list_t make_view_list(src_image_t& image)
{
	auto view = img::make_view(image);

	view_list_t list;
	img::index_t width = 0;
	img::index_t height = 0;

	auto const add_slice = [&](auto x, auto y) { list.push_back(gil::subimage_view(view, x, y, width, height)); };

	// horizontal slices of each range
	for (auto const& range : pixel_ranges)
	{
		width = (range.x_end - range.x_begin) / 2;
		height = (range.y_end - range.y_begin) / 100;

		for (auto y = range.y_begin; y < range.y_end; y += height)
		{
			for (auto x = range.x_begin; x < range.x_end; x += width)
			{
				add_slice(x, y);
			}			
		}
	}

	return list;
}


double average_shade(src_view_t const& view)
{
	auto const count = view.height() * view.width();
	double total = 0;
	auto const pred = [&](auto const& p) { total += p; };

	gil::for_each_pixel(view, pred);

	return total / count;
}


double max_shade(src_view_t const& view)
{
	double max = *(view.at(0, 0));

	auto const pred = [&](auto const& p) 
	{
		if (p > max)
			max = p;
	};

	gil::for_each_pixel(view, pred);

	return max;
}


double min_shade(src_view_t const& view)
{
	double min = *(view.at(0, 0));

	auto const pred = [&](auto const& p)
	{
		if (p < min)
			min = p;
	};

	gil::for_each_pixel(view, pred);

	return min;
}


double to_value(bits8_range_t const& range)
{
	four_bytes_t bytes;
	bytes.bytes[0] = range.min;
	bytes.bytes[1] = range.max;
	bytes.bytes[2] = 0;
	bytes.bytes[3] = 0;

	return static_cast<double>(bytes.value) / BITS32_MAX;
}



bits8_range_t to_range(double val)
{
	assert(val >= 0);
	assert(val <= 1);

	auto value = static_cast<uint32_t>(val * BITS32_MAX);

	four_bytes_t bytes;
	bytes.value = value;

	assert(bytes.bytes[0] < bytes.bytes[1]);
	assert(bytes.bytes[2] == 0);
	assert(bytes.bytes[3] == 0);

	return { bytes.bytes[0], bytes.bytes[1] };
}


double range_diff(bits8_range_t const& lhs, bits8_range_t const& rhs)
{
	auto const min = std::max(lhs.min, rhs.min);
	auto const max = std::min(lhs.max, rhs.max);

	// if the ranges overlap, no difference
	if (min <= max)
		return 0;

	return min - max;
}




double from_stats(src_view_t const& view)
{
	using shade_qty_t = unsigned;
	using color_hist_t = std::array<shade_qty_t, NUM_GRAY_SHADES>;

	// build histogram of pixel shades
	color_hist_t hist = { 0 };
	gil::for_each_pixel(view, [&](auto const& p) { ++hist[p]; });

	// calculate mean shade
	size_t qty_total = 0;
	double val_total = 0;
	for (size_t shade = 0; shade < hist.size(); ++shade)
	{
		auto qty = hist[shade];
		if (!qty)
			continue;

		qty_total += qty;
		val_total += qty * shade;
	}

	auto const mean = qty_total == 0 ? 0 : val_total / qty_total;
	assert(mean <= static_cast<double>(MAX_SHADE));

	// calculate standard deviation
	double s_total = 0;
	qty_total = 0;
	for (size_t shade = 0; shade < hist.size(); ++shade)
	{
		auto val = shade;
		auto qty = hist[shade];

		if (!qty)
			continue;

		qty_total += qty;
		auto diff = val - mean;

		s_total += qty * diff * diff;
	}

	auto const sigma = qty_total == 0 ? 0 : std::sqrt(s_total / qty_total);

	auto const min = std::max(0.0, mean - sigma);
	auto const max = std::min(static_cast<double>(MAX_SHADE), mean + sigma);

	return to_value({ static_cast<img::bits8>(min), static_cast<img::bits8>(max) });
}



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

		// scale to 24 bit value
		four_bytes_t x;
		const auto ratio = (val - DATA_MIN_VALUE) / (DATA_MAX_VALUE - DATA_MIN_VALUE);		
		x.value = static_cast<img::bits32>(ratio * BITS24_MAX);

		assert(x.bytes[3] == 0);

		const auto r = x.bytes[0];
		const auto g = x.bytes[1];
		const auto b = x.bytes[2];

		return img::to_pixel(r, g, b, MAX_SHADE); // full alpha channel
	}


	inline double data_pixel_to_data_value(data_pixel_t const& pix)
	{
		const auto rgba = img::to_rgba(pix);

		assert(rgba.a == MAX_SHADE);

		four_bytes_t x;

		
		x.bytes[0] = rgba.r;
		x.bytes[1] = rgba.g;
		x.bytes[2] = rgba.b;
		x.bytes[3] = 0;

		return static_cast<double>(x.value) / BITS24_MAX;
	}


	inline src_data_t file_to_data(const char* src_file)
	{
		gil::image_read_settings<gil::bmp_tag> read_settings;
		src_image_t image;
		gil::read_and_convert_image(src_file, image, read_settings);
		//gil::read_and_convert_image(src_file, image, gil::bmp_tag());

		auto const views = make_view_list(image);

		src_data_t data;

		//auto const pred = [](auto const& view) { return average_shade(view) / MAX_SHADE; };
		//auto const pred = [](auto const& view) { return max_shade(view) / MAX_SHADE; };
		//auto const pred = [](auto const& view) { return min_shade(view) / MAX_SHADE; };


		std::transform(views.begin(), views.end(), std::back_inserter(data), from_stats);			

		assert(data.size() == DATA_IMAGE_WIDTH);

		return data;
	}
}