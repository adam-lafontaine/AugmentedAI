#include "pixel_conversion.hpp"

#include <cassert>
#include <random>

namespace model_generator
{
	using shade_t = img::bits8;


	// how a value in a centroid is converted to a pixel for saving
	img::pixel_t to_centroid_pixel(double val, bool is_relevant)
	{
		img::bits8 min = 0;
		img::bits8 max = 255;

		const auto in_range = val >= 0 && val < 255;;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(min, max);

		// red channel used as a flag for inspector
		// if zero, value can be ignored
		const shade_t r = is_relevant && in_range ? dist(gen) : 0;

		const shade_t g = dist(gen); // doesn't matter

		// only the blue channel is used to store data
		const shade_t b = static_cast<shade_t>(std::abs(val));

		const shade_t a = max;

		return img::to_pixel(r, g, b, a);
	}


	double to_centroid_value(img::pixel_t const& p)
	{
		const auto rgba = img::to_rgba(p);
		if (!rgba.r)
			return -1;

		const double max = 255;

		// only the blue channel is used to store value
		return static_cast<double>(rgba.b);
	}


	
}