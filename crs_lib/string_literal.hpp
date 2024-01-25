#pragma once

#include <compare>
#include <string_view>
#include "std_types.hpp"

namespace crs_lib::string_literal
{
	template<class Char_, usize n_>
	struct StringLiteral final {
		Char_ value[n_]{};
		
		constexpr StringLiteral(const Char_ (&str)[n_]) noexcept {
			for(usize i = 0; i < n_; ++i)
			{
				value[i] = str[i];
			}
		}

		constexpr operator const decltype(value)&() const noexcept
		{
			return value;
		}

		explicit constexpr operator std::basic_string_view<Char_>() const noexcept
		{
			return std::basic_string_view<Char_>{value, n_ - 1};
		}

		friend constexpr auto operator<=>(const StringLiteral& lhs, const StringLiteral& rhs) = default;
		friend constexpr bool operator==(const StringLiteral& lhs, const StringLiteral& rhs) = default;

		template<usize r_>
		friend constexpr auto operator+(const StringLiteral& lhs, const StringLiteral<Char_, r_>& rhs) noexcept
		{
			StringLiteral<Char_, n_ + r_ - 1> result{};

			for(usize i = 0; i < n_ - 1; ++i)
			{
				result.value[i] = lhs.value[i];
			}

			for(usize i = 0; i < r_; ++i)
			{
				result.value[i + n_ - 1] = rhs.value[i];
			}

			return result;
		}

		static constexpr auto length() noexcept
		{
			return n_ - 1;
		}
	};

	template<StringLiteral sl>
	constexpr auto operator""_sl() noexcept
	{
		return sl;
	}
}