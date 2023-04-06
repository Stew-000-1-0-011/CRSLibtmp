#pragma once

#include <concepts>
#include "marker.hpp"

namespace CRSLib::Motor
{
	struct Normal : StateMarker
	{};
	
	template<class T>
	concept normal = std::derived_from<T, Normal>;
}