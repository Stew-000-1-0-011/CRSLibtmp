#pragma once

#include <concepts>

#include <CRSLibtmp/std_type.hpp>
#include <CRSLibtmp/string_literal.hpp>

namespace CRSLib
{
	namespace MetaType
	{
		namespace Implement
		{
			struct IdentifierMarker{};
			struct DigitIdentifierMarker : IdentifierMarker{};
			struct StringIdentifierMarker : IdentifierMarker{};
		}

		namespace Concept
		{
			template<class T>
			concept is_identifier = std::derived_from<T, Implement::IdentifierMarker>;
			template<class T>
			concept is_digit_identifier = std::derived_from<T, Implement::DigitIdentifierMarker>;
			template<class T>
			concept is_string_identifier = std::derived_from<T, Implement::StringIdentifierMarker>;
		}

		namespace Type
		{
			template<size_t identifier_>
			struct DigitIdentifier final : Implement::DigitIdentifierMarker
			{
				static constexpr auto identifier = identifier_;
			};

			template<is_string_literal auto identifier_>
			struct StringIdentifier final : Implement::StringIdentifierMarker
			{
				static constexpr auto identifier = identifier_;
			};
		}

		namespace Declaration
		{
			template<auto>
			auto id = []{static_assert([]{return false;}());};

			template<size_t identifier>
			Type::DigitIdentifier<identifier> id<identifier>{};

			template<is_string_literal auto identifier>
			Type::StringIdentifier<identifier> id<identifier>{};
		}
	}
}