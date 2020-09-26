#include "pixel_conversion.hpp"
#include "../../DataAdaptor/src/data_adaptor.hpp"

#include <cassert>
#include <random>

namespace data = data_adaptor;

namespace model_generator
{
	using shade_t = img::bits8;


	// how a value in a centroid is converted to a pixel for saving
	img::pixel_t to_centroid_pixel(double val, bool is_relevant)
	{
		assert(val >= data::data_min_value());
		assert(val <= data::data_max_value());

		img::bits8 min = 0;
		img::bits8 max = 255;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(min, max);

		// red channel used as a flag for inspector
		// if zero, value can be ignored
		const shade_t r = is_relevant ? dist(gen) : 0;

		const shade_t g = dist(gen); // doesn't matter

		// only the blue channel is used to store data
		const shade_t b = static_cast<shade_t>(std::abs(val) * max);

		const shade_t a = max;

		return img::to_pixel(r, g, b, a);
	}


	double to_centroid_value(img::pixel_t const& p)
	{
		const auto rgba = img::to_rgba(p);
		if (!rgba.r)
			return 0;

		const double max = 255;

		// only the blue channel is used to store value
		return rgba.b / max;
	}


	
}