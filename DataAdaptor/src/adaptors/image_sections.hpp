#pragma once

#include "../data_adaptor.hpp"
#include "../../../utils/libimage/libimage.hpp"

#include <cassert>

#ifdef __linux

#define sprintf_s sprintf

#endif

namespace img = libimage;


using feature_pixel_t = data_adaptor::feature_pixel_t;
using features_t = data_adaptor::features_t;

constexpr size_t HORIZONTAL_SECTIONS = 16;
constexpr size_t VERTICAL_SECTIONS = 16;
constexpr size_t MAX_FEATURE_IMAGE_SIZE = 300000;
constexpr auto BITS32_MAX = UINT32_MAX;


namespace impl
{
	constexpr size_t FEATURE_IMAGE_WIDTH = HORIZONTAL_SECTIONS * VERTICAL_SECTIONS;
	constexpr r64 FEATURE_MIN_VALUE = 0;
	constexpr r64 FEATURE_MAX_VALUE = 1;


	// Define how to name save files
	inline std::string make_numbered_file_name(u32 index, size_t index_length)
	{
		index_length = index_length < 2 ? 2 : index_length;

		char idx_str[10];
		sprintf_s(idx_str, "%0*d", (int)index_length, index); // zero pad index number

		return std::string(idx_str) + data_adaptor::FEATURE_IMAGE_EXTENSION;
	}


	//======= TODO: IMPLEMENT =================

	inline feature_pixel_t value_to_feature_pixel(r64 val)
	{
		assert(val >= FEATURE_MIN_VALUE);
		assert(val <= FEATURE_MAX_VALUE);

		const auto ratio = (val - FEATURE_MIN_VALUE) / (FEATURE_MAX_VALUE - FEATURE_MIN_VALUE);

		img::pixel_t color;
		color.value = static_cast<u32>(ratio * BITS32_MAX);

		return color.value;
	}


	inline r64 feature_pixel_to_value(feature_pixel_t const& pix)
	{		
		return static_cast<r64>(pix) / BITS32_MAX;
	}


	inline features_t file_to_features(const char* src_file)
	{
		img::image_t image;
		img::read_image_from_file(src_file, image);

		img::image_t resized;
		resized.width = HORIZONTAL_SECTIONS;
		resized.height = VERTICAL_SECTIONS;
		auto view = img::make_resized_view(image, resized);

		features_t data;

		std::transform(view.begin(), view.end(), std::back_inserter(data), feature_pixel_to_value);

		assert(data.size() == FEATURE_IMAGE_WIDTH);

		return data;
	}
}
