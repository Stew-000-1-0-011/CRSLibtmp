#pragma once

#include <utility>
#include <CRSLibtmp/Can/utility.hpp>

namespace CRSLib::Can::MainPC
{
	template<class T>
	concept pillarbox = std::move_constructible<T> && requires(T mut, const DataField data)
	{
		mut.post(data);
	};
}
