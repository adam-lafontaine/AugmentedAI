#pragma once

#include "../../utils/libimage.hpp"

namespace img = libimage;

namespace model_generator
{
	// how a value in a centroid is converted to a pixel for saving
	img::pixel_t to_centroid_pixel(double val, bool is_relevant = true);

	// how a pixel in a saved model is converted to a centroid value
	double to_centroid_value(img::pixel_t const& p);

}