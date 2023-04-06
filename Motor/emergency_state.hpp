#pragma once

#include <concepts>
#include "marker.hpp"

namespace CRSLib::Motor
{
	struct Emergency : StateMarker
	{};
	
	template<class T>
	concept emergency = std::derived_from<T, Emergency>;
}