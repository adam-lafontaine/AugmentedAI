#include "pixel_conversion.hpp"

#include <cassert>
#include <random>


constexpr auto BITS32_MAX = UINT32_MAX;

constexpr auto MODEL_VALUE_MIN = 0.0;
constexpr auto MODEL_VALUE_MAX = BITS32_MAX;

using shade_t = u8;

constexpr shade_t PIXEL_ACTIVE = 255;
constexpr shade_t PIXEL_INACTIVE = 254;

namespace model_generator
{
	bool is_relevant(model_value_t model_val)
	{
		return model_val >= MODEL_VALUE_MIN && model_val <= MODEL_VALUE_MAX;
	}


	model_value_t data_pixel_to_model_value(data_pixel_t const& data_pix)
	{
		return static_cast<r64>(data_pix.value);
	}


	model_pixel_t model_value_to_model_pixel(model_value_t model_val, bool is_active)
	{
		assert(is_relevant(model_val)); // only valid values can be converted to a pixel

		shade_t min = 0;
		shade_t max = 255;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(min, max);

		
		// red channel doesn't matter
		const shade_t r = dist(gen);

		// green channel doesn't matter
		const shade_t g = dist(gen);

		// only the blue channel is used to store data
		// 32 bit number converted to 8 bit, loss of precision
		const auto ratio = model_val / MODEL_VALUE_MAX;
		const shade_t b = static_cast<shade_t>(ratio * max);

		// use alpha channel to flag if pixel is used in the model
		const shade_t a = is_active ? PIXEL_ACTIVE : PIXEL_INACTIVE;

		return img::to_pixel(r, g, b, a);
	}


	model_value_t model_pixel_to_model_value(model_pixel_t const& model_pix)
	{
		const auto rgba = model_pix;

		// if alpha channel has been flagged as inactive,
		// return value makes is_relevant() == false
		if (rgba.alpha != PIXEL_ACTIVE)
			return MODEL_VALUE_MIN - 1;

		// only the blue channel is used to store a value
		// the red and green channels are random values used to make the model image look more interesting
		const auto ratio = rgba.blue / 255.0;
		return ratio * MODEL_VALUE_MAX;
	}
}