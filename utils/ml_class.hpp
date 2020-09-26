#pragma once

#include <cstddef>

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
