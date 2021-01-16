#pragma once

#include <cstddef>
#include <functional>

/*

Enumeration for referring to classes.
Values before MLClass::Count are used as the array indeces when clustering.
MLClass::Count is used for loop counting etc.
Define as many classes as you like.

*/

enum class MLClass : size_t
{
	Fail = 0,
	Pass,

	/* Add other classes here */

	Count,

	/* Add classes for handling special cases */
	Unknown,
	Error
};


// Convert an MLClass to an array index
constexpr size_t to_class_index(MLClass c) { return static_cast<size_t>(c); }


// The number of classes for classification
constexpr size_t ML_CLASS_COUNT = to_class_index(MLClass::Count);



// make a separate namespace to show where this function comes from
namespace mlclass
{
	// Pass a function for iterating over all of the classes.
	// Using the same for loop over and over again gets annoying.
	using class_func_t = std::function<void(size_t c)>;
	inline void for_each_class(class_func_t const& func)
	{
		for (size_t class_index = 0; class_index < ML_CLASS_COUNT; ++class_index)
		{
			func(class_index);
		}
	}
}



