#pragma once

#include <cstddef>
#include <functional>

//====== CLASS LABELS ===========================

enum class MLClass : size_t
{
	Fail = 0,
	Pass,
	Count,
	Unknown,
	Error
};

constexpr size_t to_class_index(MLClass c) { return static_cast<size_t>(c); }

constexpr size_t ML_CLASS_COUNT = to_class_index(MLClass::Count);


using class_func_t = std::function<void(size_t c)>;

inline void for_each_class(class_func_t const& func)
{
	for (size_t class_index = 0; class_index < ML_CLASS_COUNT; ++class_index)
	{
		func(class_index);
	}
}
