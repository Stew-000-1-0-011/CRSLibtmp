// コメント程度の効力しかない。

#pragma once

#include <type_traits>
#include <numbers>

namespace CRSLib::Math
{
	template<class T, T min, T max>
	requires std::is_arithmetic_v<T>
	struct RangedScalar final
	{
		T value;

		operator T&() noexcept
		{
			return value;
		}

		operator T() const noexcept
		{
			return value;
		}
	};

	template<class T>
	requires std::is_arithmetic_v<T>
	using Degree = RangedScalar<T, static_cast<T>(0), static_cast<T>(360)>;
}