#pragma once

#include "../../utils/cluster.hpp"

/*

Used to build a function for calculating distance between a feature vector and a centroid for clustering purposes.
To save time, another part of the program finds which indeces of the data actually contribute to the
 the result and only those indeces are considered.
Any function can be used here as appropriate for the application.

*/

namespace model_generator
{
	using index_list_t = std::vector<size_t>;

	
	inline cluster::dist_func_t build_cluster_distance(index_list_t const& relevant_indeces)
	{
		// average absolute difference
		return [&](auto const& data, auto const& centroid)
		{
			r64 total = 0;

			for (auto i : relevant_indeces)
			{
				total += std::abs(data[i] - centroid[i]);
			}

			return total / relevant_indeces.size();
		};


		/*

		// Root mean square difference
		return [&](auto const& data, auto const& centroid)
		{
			r64 total = 0;

			for (auto i : relevant_indeces)
			{
				auto const diff = data[i] - centroid[i];
				total += diff * diff;
			}

			auto const mean_square = total / relevant_indeces.size();

			return std::sqrt(mean_square);
		};

		*/
	}

}