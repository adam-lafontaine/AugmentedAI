#include "pixel_conversion.hpp"
//#include "../../DataAdaptor/src/data_adaptor.hpp"

#include <cassert>
#include <random>

//namespace data = data_adaptor;


constexpr auto BITS32_MAX = static_cast<double>(img::to_bits32(255, 255, 255, 255));
constexpr double BITS8_MAX = 255;

constexpr auto MODEL_VALUE_MIN = 0.0;
constexpr auto MODEL_VALUE_MAX = BITS32_MAX;

using shade_t = img::bits8;

constexpr shade_t ACTIVE = 255;
constexpr shade_t INACTIVE = 254;

namespace model_generator
{
	


	bool is_relevant(double model_val)
	{
		return model_val >= MODEL_VALUE_MIN && model_val <= MODEL_VALUE_MAX;
	}


	double data_pixel_to_model_value(data_pixel_t const& data_pix)
	{
		return static_cast<double>(img::to_bits32(data_pix));
	}


	model_pixel_t model_value_to_model_pixel(double model_val, bool is_active)
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

		// use alpha channel to flag is pixel is used in the model
		const shade_t a = is_active ? ACTIVE : INACTIVE;

		return img::to_pixel(r, g, b, a);
	}


	double model_pixel_to_model_value(model_pixel_t const& model_pix)
	{
		const auto rgba = img::to_rgba(model_pix);

		// if alpha channel has been flagged as inactive,
		// return value makes is_relevant() == false
		if (rgba.a == INACTIVE)
			return MODEL_VALUE_MIN - 1;

		const double max = 255;

		// only the blue channel is used to store value
		const auto ratio = rgba.b / BITS8_MAX;
		return ratio * MODEL_VALUE_MAX;
	}
}