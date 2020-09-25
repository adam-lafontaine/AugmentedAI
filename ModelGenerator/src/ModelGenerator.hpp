#pragma once

#include "../../utils/ml_class.hpp"

#include <string>
#include <array>
#include <vector>


class ModelGenerator
{
public:
	using file_path_t = std::string;
	using file_list_t = std::vector<file_path_t>;
	using class_file_list_t = std::array<file_list_t, ML_CLASS_COUNT>;

private:
	

	// file paths of raw data images by class
	class_file_list_t m_class_data;

	// for cleaning up after reading data
	void purge_class_data();

	/*
	index_list_t m_data_indeces;

	// cluster properties
	cluster_t m_cluster;
	centroid_list_t m_centroids;
	std::array<size_t, ML_CLASS_COUNT> m_class_clusters = { 10, 10 };
	*/

public:
	// reads directory of raw data for a given class
	void set_teach_files(const char* src_dir, MLClass class_index);

	// saves properties based on all of the data read
	void teach_and_save(const char* save_dir);

	//void save_learned(const char* save_dir);

};