#pragma once

#include "config_reader.hpp"

#include <string>
#include <algorithm>

namespace cr = config_reader;

#ifdef _WIN32

//#define TEST_FILE_ROOT "D:/test_images"

//#define PROJECT_ROOT "D:/repos/AugmentedAI/DataAdaptor/test"



#else

#define TEST_FILE_ROOT "/home/adam/projects/test_images"

#define PROJECT_ROOT "/home/adam/projects/AugmentedAI/DataAdaptor/test"

#endif

// TODO: test
//#define SRC_FAIL_ROOT TEST_FILE_ROOT "/src_fail"
//#define SRC_PASS_ROOT TEST_FILE_ROOT "/src_pass"

// TODO: test
//#define DATA_FAIL_ROOT TEST_FILE_ROOT "/data_fail"
//#define DATA_PASS_ROOT TEST_FILE_ROOT "/data_pass"

//#define ALPHA_PATH TEST_FILE_ROOT "/alphabet/alphabet.png"
//#define BORDER_PATH TEST_FILE_ROOT "/alphabet/border.png"

//#define MODEL_ROOT TEST_FILE_ROOT "/model"

enum class TestDir : size_t
{
	PROJECT_ROOT = 0,
	SRC_FAIL_ROOT,
	SRC_PASS_ROOT,
	DATA_FAIL_ROOT,
	DATA_PASS_ROOT,
	MODEL_ROOT,
	ALPHA_PATH,
	BORDER_PATH,	
	Count
};


constexpr auto TEST_DIR_CONFIG_PATH = "test_dir_config.txt";


class TestDirConfig
{
private:

	cr::config_t m_config;
	cr::key_list_t m_keys;

	size_t to_index(TestDir dir) { return static_cast<size_t>(dir); }


public:

	TestDirConfig()
	{
		m_keys = cr::key_list_t(static_cast<size_t>(TestDir::Count));
		m_keys[to_index(TestDir::PROJECT_ROOT)] =   "PROJECT_ROOT";
		m_keys[to_index(TestDir::SRC_FAIL_ROOT)] =  "SRC_FAIL_ROOT";
		m_keys[to_index(TestDir::SRC_PASS_ROOT)] =  "SRC_PASS_ROOT";
		m_keys[to_index(TestDir::DATA_FAIL_ROOT)] = "DATA_FAIL_ROOT";
		m_keys[to_index(TestDir::DATA_PASS_ROOT)] = "DATA_PASS_ROOT";
		m_keys[to_index(TestDir::MODEL_ROOT)] =     "MODEL_ROOT";
		m_keys[to_index(TestDir::ALPHA_PATH)] =     "ALPHA_PATH";		
		m_keys[to_index(TestDir::BORDER_PATH)] =    "BORDER_PATH";

		m_config = cr::read_config(TEST_DIR_CONFIG_PATH, m_keys);
	}


	std::string get(TestDir dir)
	{
		return m_config[m_keys[to_index(dir)]];
	}

	
	bool has_all_keys() 
	{
		return m_config.size() == m_keys.size();
	}
};