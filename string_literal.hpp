#pragma once

#include <concepts>

#include "std_type.hpp"

namespace CRSLib
{
	template<class CharT, size_t n>
	struct StringLiteral final
	{
		CharT value[n]{};
		
		constexpr StringLiteral(const CharT (&str)[n]) noexcept
		{
			for(size_t i = 0; i < n; ++i)
			{
				value[i] = str[i];
			}
		}

		constexpr operator const decltype(value)&() const noexcept
		{
			return value;
		}

		friend constexpr auto operator<=>(const StringLiteral& lhs, const StringLiteral& rhs) = default;
		friend constexpr bool operator==(const StringLiteral& lhs, const StringLiteral& rhs) = default;

		template<size_t l, size_t r>
		friend constexpr auto operator+(const StringLiteral<CharT, l>& lhs, const StringLiteral<CharT, r>& rhs) noexcept
		{
			StringLiteral<CharT, l + r - 1> result{};

			for(size_t i = 0; i < l - 1; ++i)
			{
				result.value[i] = lhs.value[i];
			}

			for(size_t i = 0; i < r; ++i)
			{
				result.value[i + l - 1] = rhs.value[i];
			}

			return result;
		}
	};

	template<StringLiteral sl>
	constexpr auto operator""_sl() noexcept
	{
		return sl;
	}
}