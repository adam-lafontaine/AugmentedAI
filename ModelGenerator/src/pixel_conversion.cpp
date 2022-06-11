#include "pixel_conversion.hpp"

#include <cassert>


constexpr auto BITS32_MAX = UINT32_MAX;

constexpr u32 CHANNEL_3_MAX = 255 * 255 * 255;

constexpr auto MODEL_VALUE_MIN = 0.0;
constexpr auto MODEL_VALUE_MAX = (r64)CHANNEL_3_MAX;

constexpr u8 PIXEL_ACTIVE = 255;
constexpr u8 PIXEL_INACTIVE = 254;

namespace model_generator
{
	bool is_relevant(r64 model_val)
	{
		return model_val >= MODEL_VALUE_MIN && model_val <= MODEL_VALUE_MAX;
	}


	r64 feature_pixel_to_model_value(data_pixel_t const& data_pix)
	{
		auto ratio = (r64)(data_pix.value) / BITS32_MAX;

		return MODEL_VALUE_MIN + ratio * (MODEL_VALUE_MAX - MODEL_VALUE_MIN);
	}


	model_pixel_t model_value_to_model_pixel(r64 model_val, bool is_active)
	{
		assert(is_relevant(model_val)); // only valid values can be converted to a pixel

		auto const ratio = (model_val - MODEL_VALUE_MIN) / (MODEL_VALUE_MAX - MODEL_VALUE_MIN);

		auto rgb_value = (u32)(ratio * CHANNEL_3_MAX);

		auto r = rgb_value & 0b0000'0000'0000'0000'0000'0000'1111'1111;
		auto g = (rgb_value & 0b0000'0000'0000'0000'1111'1111'0000'0000) >> 8;
		auto b = (rgb_value & 0b0000'0000'1111'1111'0000'0000'0000'0000) >> 16;

		model_pixel_t p{};
		p.red = r;
		p.green = g;
		p.blue = b;

		p.alpha = is_active ? PIXEL_ACTIVE : PIXEL_INACTIVE;

		return p;
	}


	r64 model_pixel_to_model_value(model_pixel_t const& model_pix)
	{
		// if alpha channel has been flagged as inactive,
		// return value makes is_relevant() == false
		if (model_pix.alpha != PIXEL_ACTIVE)
		{
			return MODEL_VALUE_MIN - 1;
		}

		auto r = (u32)model_pix.red;
		auto g = (u32)model_pix.green;
		auto b = (u32)model_pix.blue;

		auto rgb_value = b << 16 | g << 8 | r;

		auto ratio = (r64)rgb_value / CHANNEL_3_MAX;

		return MODEL_VALUE_MIN + ratio * (MODEL_VALUE_MAX - MODEL_VALUE_MIN);
	}
}