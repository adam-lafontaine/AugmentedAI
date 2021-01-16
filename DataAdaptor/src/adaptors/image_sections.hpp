#pragma once

#include "../data_adaptor.hpp"

#ifdef __linux

#define sprintf_s sprintf

#endif


using data_pixel_t = data_adaptor::data_pixel_t;
using src_data_t = data_adaptor::src_data_t;

constexpr size_t HORIZONTAL_SECTIONS = 16;
constexpr size_t VERTICAL_SECTIONS = 16;
constexpr size_t MAX_DATA_IMAGE_SIZE = 300000;
constexpr auto BITS32_MAX = img::to_bits32(255, 255, 255, 255);


// for getting bytes from 32 bit values
union four_bytes_t
{
	img::bits32 value;
	img::bits8 bytes[4];
};





namespace impl
{
	constexpr size_t DATA_IMAGE_WIDTH = HORIZONTAL_SECTIONS * VERTICAL_SECTIONS;
	constexpr double DATA_MIN_VALUE = 0;
	constexpr double DATA_MAX_VALUE = 1;


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
		auto image = img::read_image_from_file(src_file);
		auto resized = img::image_t(HORIZONTAL_SECTIONS, VERTICAL_SECTIONS);
		const auto view = img::make_resized_view(image, resized);

		src_data_t data;
		data.reserve(DATA_IMAGE_WIDTH);

		const auto add_data = [&](const auto& pix) { data.push_back(data_pixel_to_data_value(pix)); };

		boost::gil::for_each_pixel(view, add_data);

		assert(data.size() == DATA_IMAGE_WIDTH);

		return data;
	}
}
