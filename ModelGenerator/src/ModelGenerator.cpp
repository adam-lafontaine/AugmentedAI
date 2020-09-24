#include "ModelGenerator.hpp"
#include "../../utils/cluster_config.hpp"
#include "../../utils/libimage.hpp"
#include "../../utils/dirhelper.hpp"

#include <cassert>
#include <algorithm>
#include <iterator>
#include <functional>
#include <cmath>
#include <random>
#include <ctime>

namespace dir = dirhelper;
namespace img = libimage;
namespace gil = boost::gil;


constexpr auto DATA_IMAGE_WIDTH = 256; // TODO: get from DataAdaptor

using shade_qty_t = unsigned;

// provides a count for every shade that is found
using shade_hist_t = std::array<shade_qty_t, DATA_IMAGE_WIDTH>;

// provides shade counts for every position in the data image
using position_hists_t = std::vector<shade_hist_t>;

// shade counts by position for each class
using class_position_hists_t = std::array<position_hists_t, ML_CLASS_COUNT>;

using cluster_t = cluster::Cluster;
using centroid_list_t = cluster::value_row_list_t;

using data_list_t = std::vector<cluster::data_row_t>;
using class_cluster_data_t = std::array<data_list_t, ML_CLASS_COUNT>;

using index_list_t = std::vector<size_t>;

using file_path_t = ModelGenerator::file_path_t;



//======= HELPERS ===================

using class_func_t = std::function<void(size_t c)>;
void for_each_class(class_func_t const& func)
{
	for (size_t class_index = 0; class_index < ML_CLASS_COUNT; ++class_index)
	{
		func(class_index);
	}
}


// finds the indeces of the data that contribute to determining the class
// compares the average of shades with observed values
// does not account for multiple maxima
static index_list_t find_positions(class_position_hists_t const& class_pos_hists)
{
	const double min_diff = 6.0;
	const size_t num_pos = class_pos_hists[0].size();
	std::array<double, ML_CLASS_COUNT> class_avg = { 0.0 };

	index_list_t list;

	for (size_t pos = 0; pos < num_pos; ++pos)
	{
		for (size_t class_index = 0; class_index < ML_CLASS_COUNT; ++class_index)
		{
			double total = 0;
			size_t count = 0;

			auto hist = class_pos_hists[class_index][pos];

			for (size_t shade = 0; shade < hist.size(); ++shade)
			{
				auto qty = hist[shade];
				if (!qty)
					continue;

				total += shade;
				++count;
			}

			class_avg[class_index] = total / count;
		}

		const auto [min, max] = std::minmax_element(class_avg.begin(), class_avg.end());
		if ((*max - *min) > min_diff)
			list.push_back(pos);

		class_avg = { 0.0 };
	}

	assert(!list.empty());

	return list;
}


//======= HISTOGRAM HELPERS ============================

static void update_properties(position_hists_t& pos, data_list_t& data, file_path_t const& file)
{
	auto image = img::read_image_from_file(file);
	auto view = img::make_view(image);

	auto const width = image.width();
	auto const height = image.height();

	assert(width == DATA_IMAGE_WIDTH);

	for (auto w = 0; w < width; ++w)
	{
		const auto column_view = img::column_view(view, w);

		gil::for_each_pixel(column_view, [&](auto const& p) { ++pos[w][data::to_data(p)]; });
	}

	for (auto y = 0; y < height; ++y)
	{
		const auto row_view = img::row_view(view, y);
		data.push_back(data::to_shade_list(row_view)); // TODO: DataAdaptor
	}
}




//======= CLASS METHODS ==================

void ModelGenerator::purge_class_data()
{
	const auto pred = [&](auto c) { m_class_data[c].clear(); };
	for_each_class(pred);
}