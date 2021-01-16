#pragma once
/*

Copyright (c) 2021 Adam Lafontaine

*/

#include <cstddef>
#include <functional>
#include <array>

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


// make a separate namespace to be explicit about where these functions comes from
namespace mlclass
{
	// Convert an MLClass to an array index
	constexpr size_t to_class_index(MLClass c) { return static_cast<size_t>(c); }


	// Convert an array index to a MLClass
	constexpr MLClass to_class(size_t index) { return static_cast<MLClass>(index); }


	// The number of classes for classification
	constexpr size_t ML_CLASS_COUNT = to_class_index(MLClass::Count);


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


	// Make a size_t array where each element index is a class index
	//  and each element is the number of clusters in that class
	// Must have the correct number elements.
	// In this case there are two elements, Fail and Pass.
	// Each class will have the same number of clusters.
	inline std::array<size_t, ML_CLASS_COUNT> make_class_clusters(size_t clusters_per_class)
	{
		return std::array<size_t, ML_CLASS_COUNT> { clusters_per_class, clusters_per_class };
	}
}



