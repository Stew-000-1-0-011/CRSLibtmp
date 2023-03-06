#pragma once

#include <concepts>

#include "std_type.hpp"

namespace CRSLib
{
	namespace Implement
	{
		struct StringLiteralMarker{};
	}

	template<class CharT, size_t n>
	struct StringLiteral final : Implement::StringLiteralMarker
	{
		CharT value[n]{};
		
		constexpr StringLiteral(const CharT (&str)[n])
		{
			for(size_t i = 0; i < n; ++i)
			{
				value[i] = str[i];
			}
		}
	};

	template<class T>
	concept is_string_literal = std::derived_from<T, Implement::StringLiteralMarker>;
}
