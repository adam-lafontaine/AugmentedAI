#include "pixel_conversion.hpp"
//#include "../../DataAdaptor/src/data_adaptor.hpp"

#include <cassert>
#include <random>

//namespace data = data_adaptor;


constexpr auto BITS32_MAX = static_cast<double>(img::to_bits32(255, 255, 255, 255));
constexpr double BITS8_MAX = 255;

constexpr auto MODEL_VALUE_MIN = 0.0;
constexpr auto MODEL_VALUE_MAX = BITS32_MAX;

namespace model_generator
{
	using shade_t = img::bits8;


	bool is_relevant(double model_val)
	{
		return model_val >= MODEL_VALUE_MIN && model_val <= MODEL_VALUE_MIN;
	}


	double data_pixel_to_model_value(data_pixel_t const& data_pix)
	{
		return static_cast<double>(img::to_bits32(data_pix));
	}


	model_pixel_t model_value_to_model_pixel(double model_val, bool is_active)
	{
		shade_t min = 0;
		shade_t max = 255;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(min, max);

		// red channel used as a flag for inspector
		// if zero, value can be ignored
		const shade_t r = is_active && is_relevant(model_val) ? dist(gen) : 0;

		// green channel doesn't matter
		const shade_t g = dist(gen);

		// only the blue channel is used to store data
		const auto ratio = (model_val - MODEL_VALUE_MIN) / (MODEL_VALUE_MAX - MODEL_VALUE_MIN);
		const shade_t b = static_cast<shade_t>(ratio * max);

		const shade_t a = max;

		return img::to_pixel(r, g, b, a);
	}


	double model_pixel_to_model_value(model_pixel_t const& model_pix)
	{
		const auto rgba = img::to_rgba(model_pix);

		// if red channel is 0, then pixel has been flagged as not relevant
		if (!rgba.r)
			return MODEL_VALUE_MIN - 1;

		const double max = 255;

		// only the blue channel is used to store value
		const auto ratio = rgba.b / max;
		return MODEL_VALUE_MIN + ratio * (MODEL_VALUE_MAX - MODEL_VALUE_MIN);
	}
}