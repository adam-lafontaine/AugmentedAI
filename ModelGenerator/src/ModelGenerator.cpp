#include "ModelGenerator.hpp"
#include "../../utils/cluster_config.hpp"


constexpr auto NUM_GRAY_SHADES = 256;

using shade_qty_t = unsigned;

// provides a count for every shade that is found
using shade_hist_t = std::array<shade_qty_t, NUM_GRAY_SHADES>;

// provides shade counts for every position in the data image
using position_hists_t = std::vector<shade_hist_t>;

// shade counts by position for each class
using class_position_hists_t = std::array<position_hists_t, ML_CLASS_COUNT>;

using cluster_t = cluster::Cluster;
using centroid_list_t = cluster::value_row_list_t;

using data_list_t = std::vector<cluster::data_row_t>;
using class_cluster_data_t = std::array<data_list_t, ML_CLASS_COUNT>;