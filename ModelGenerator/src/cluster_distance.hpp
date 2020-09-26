#pragma once

#include "../../utils/cluster.hpp"

namespace model_generator
{
	using index_list_t = std::vector<size_t>;

	// build function for evaluating distance between data and a cluster centroid
	inline cluster::dist_func_t build_cluster_distance(index_list_t const& relevant_indeces)
	{
		return [&](auto const& data, auto const& centroid)
		{
			double total = 0;

			for (auto i : relevant_indeces)
			{
				const auto lhs = data[i];
				const auto rhs = centroid[i];
				total += std::abs(lhs - rhs);
			}

			return total / relevant_indeces.size();
		};
	}
}