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
		shade_t min = 0;
		shade_t max = 255;

		const auto data_min = data::data_min_value();
		const auto data_max = data::data_max_value();

		const auto in_range = val >= data_min && val <= data_max;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(min, max);

		// red channel used as a flag for inspector
		// if zero, value can be ignored
		const shade_t r = is_relevant && in_range ? dist(gen) : 0;

		const shade_t g = dist(gen); // doesn't matter		

		// only the blue channel is used to store data
		const auto ratio = (val - data_min) / (data_max - data_min);		
		const shade_t b = static_cast<shade_t>(ratio * max);

		const shade_t a = max;

		return img::to_pixel(r, g, b, a);
	}


	double to_centroid_value(img::pixel_t const& p)
	{
		const auto data_min = data::data_min_value();
		const auto data_max = data::data_max_value();

		const auto rgba = img::to_rgba(p);
		if (!rgba.r)
			return data_min - 1;		

		const double max = 255;

		// only the blue channel is used to store value
		const auto ratio = rgba.b / max;		
		return data_min + ratio * (data_max - data_min);
	}


	bool is_relevant(double val)
	{
		return val >= data::data_min_value() && val <= data::data_max_value();
	}


	
}