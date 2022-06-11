#include "pixel_conversion.hpp"

#include <cassert>


constexpr auto BITS32_MAX = UINT32_MAX;

constexpr u32 CHANNEL_3_MAX = 255 * 255 * 255;

constexpr auto MODEL_VALUE_MIN = 0.0;
constexpr auto MODEL_VALUE_MAX = (r64)BITS32_MAX;

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
		return (r64)(data_pix.value);
	}


	model_pixel_t model_value_to_model_pixel(r64 model_val, bool is_active)
	{
		assert(is_relevant(model_val)); // only valid values can be converted to a pixel

		u8 min = 0;
		u8 max = 255;
		
		// red channel doesn't matter
		const u8 r = 105;

		// green channel doesn't matter
		const u8 g = 205;

		// only the blue channel is used to store data
		// 32 bit number converted to 8 bit, loss of precision
		auto const ratio = model_val / MODEL_VALUE_MAX;
		auto const b = (u8)(ratio * max);

		// use alpha channel to flag if pixel is used in the model
		const u8 a = is_active ? PIXEL_ACTIVE : PIXEL_INACTIVE;

		return img::to_pixel(r, g, b, a);
	}


	r64 model_pixel_to_model_value(model_pixel_t const& model_pix)
	{
		auto const rgba = model_pix;

		// if alpha channel has been flagged as inactive,
		// return value makes is_relevant() == false
		if (rgba.alpha != PIXEL_ACTIVE)
			return MODEL_VALUE_MIN - 1;

		// only the blue channel is used to store a value
		// the red and green channels are random values used to make the model image look more interesting
		auto const ratio = rgba.blue / 255.0;
		return ratio * MODEL_VALUE_MAX;
	}
}