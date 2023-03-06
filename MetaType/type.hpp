/**
 * @file xml_rpc.hpp
 * @author Stew (you@domain.com)
 * @brief xml-rpc
 * @version 0.1
 * @date 2023-02-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include <concepts>
#include <type_traits>
#include <string>
#include <variant>
#include <tuple>
#include <array>
#include <vector>
#include <algorithm>
#include <limits>

#include <CRSLibtmp/string_literal.hpp>
#include <CRSLibtmp/std_type.hpp>
#include <CRSLibtmp/utility.hpp>

#include "identifier.hpp"

namespace CRSLib
{
	namespace MetaType
	{
		namespace Marker
		{
			struct TypeMarker{};
			struct PrimitiveMarker : TypeMarker{};
			struct NullMarker : TypeMarker{};
			struct TupleMarker : TypeMarker{};
			struct VectorMarker : TypeMarker{};
			struct ArrayMarker : VectorMarker{};
			struct ItemMarker : TypeMarker{};
			struct RecordMarker : TypeMarker{};
			struct VariantMarker : TypeMarker{};
		}

		namespace Concept
		{
			template<class T>
			concept is_type = std::derived_from<T, Marker::TypeMarker>;
			template<class T>
			concept is_primitive = std::derived_from<T, Marker::PrimitiveMarker>;
			template<class T>
			concept is_null = std::derived_from<T, Marker::NullMarker>;
			template<class T>
			concept is_tuple = std::derived_from<T, Marker::TupleMarker>;
			template<class T>
			concept is_vector = std::derived_from<T, Marker::VectorMarker>;
			template<class T>
			concept is_array = std::derived_from<T, Marker::ArrayMarker>;
			template<class T>
			concept is_item = std::derived_from<T, Marker::ItemMarker>;
			template<class T>
			concept is_record = std::derived_from<T, Marker::RecordMarker>;
			template<class T>
			concept is_variant = std::derived_from<T, Marker::VariantMarker>;
		}

		namespace Implement
		{
			template<Concept::is_type Type>
			struct ValueInner;
		}

		template<class T>
		using Value = Implement::ValueInner<std::remove_cvref_t<T>>;

#define Stew_define_primitive(PrimitiveType, primitive_template_variable, ValueType) \
		namespace Type{struct PrimitiveType final : Marker::PrimitiveMarker{};}\
		template<>\
		struct Implement::ValueInner<Type::PrimitiveType> final\
		{\
			ValueType v{};\
		};\
		namespace Declaration{Type::PrimitiveType primitive_template_variable{};}

		Stew_define_primitive(Boolean, boolean, bool);
		Stew_define_primitive(I32, i32, CRSLib::i32);
		Stew_define_primitive(F64, f64, double);
		Stew_define_primitive(String, string, std::string);
		Stew_define_primitive(Byte, byte, CRSLib::byte);

#undef Stew_define_primitive

		namespace Type{struct Null final : Marker::NullMarker{};}
		namespace Declaration{constexpr Type::Null null{};}
		template<>
		struct Implement::ValueInner<Type::Null> final
		{};


		namespace Type
		{
			template<Concept::is_type auto ...>
			struct Tuple final : Marker::TupleMarker{};
		}

		namespace Declaration
		{
			template<Concept::is_type auto ... types>
			constexpr Type::Tuple<types ...> tuple{};
		}

		template<Concept::is_type auto ... types>
		struct Implement::ValueInner<Type::Tuple<types ...>> final
		{
			std::tuple<Value<decltype(types)>...> v{};

			auto& operator/(Concept::is_digit_identifier auto index)
			{
				return std::get<index.identifier>(v);
			}
		};


		namespace Type
		{
			template<Concept::is_type auto>
			struct Vector final : Marker::VectorMarker{};
		}

		namespace Declaration
		{
			template<Concept::is_type auto type>
			constexpr Type::Vector<type> vector{};
		}

		template<Concept::is_type auto type>
		struct Implement::ValueInner<Type::Vector<type>> final
		{
			std::vector<Value<decltype(type)>> v{};
		};


		namespace Type
		{
			template<Concept::is_type auto, size_t>
			struct Array final : Marker::ArrayMarker{};
		}

		namespace Declaration
		{
			template<Concept::is_type auto type, size_t size>
			constexpr Type::Array<type, size> array{};
		}

		template<Concept::is_type auto type, size_t size>
		struct Implement::ValueInner<Type::Array<type, size>> final
		{
			std::array<Value<decltype(type)>, size> v{};
		};


		namespace Type
		{
			template<Concept::is_string_identifier auto identifier_, Concept::is_type auto type_>
			struct Item final : Marker::ItemMarker
			{
				static constexpr auto identifier = identifier_;
				static constexpr auto type = type_;
			};
		}

		namespace Declaration
		{
			template<Concept::is_string_identifier auto identifier, Concept::is_type auto type>
			constexpr Type::Item<identifier, type> item{};
		}

		namespace Implement
		{
			template<Concept::is_item auto ... items>
			constexpr bool has_duplicated =
			(
				[]<Concept::is_item auto ... items2>(Concept::is_item auto item)
				{
					return (static_cast<size_t>(std::same_as<decltype(item.identifier), decltype(items2.identifier)>) + ...);
				}.template operator()<items ...>(items) + ...
			) != sizeof...(items);

			template<Concept::is_string_identifier auto identifier, Concept::is_item auto ... items>
			constexpr size_t index_of =
			[]<size_t ... indices>(const std::index_sequence<indices ...>)
			{
				return std::min((identifier == items.identifier ? indices : std::numeric_limits<size_t>::max()) ...);
			}(std::index_sequence_for<decltype(items) ...>());
		}

		template<Concept::is_item auto ... items>
		struct Record final : Marker::RecordMarker
		{
			static_assert(!Implement::has_duplicated<items ...>, "There is a duplication in identifier.");
		};

		namespace Declaration
		{
			template<Concept::is_item auto ... items>
			constexpr Record<items ...> record{};
		}

		template<Concept::is_item auto ... items>
		struct Implement::ValueInner<Record<items ...>> final
		{
			std::tuple<Value<decltype(items.type)>...> v{};

			auto& operator/(Concept::is_string_identifier auto member)
			{
				return std::get<Implement::index_of<member.identifier, items ...>>(v);
			}
		};


		template<Concept::is_item auto ... items>
		struct Variant final : Marker::VariantMarker
		{
			static_assert(!Implement::has_duplicated<items ...>, "There is a duplication in identifier.");
		};

		namespace Declaration
		{
			template<Concept::is_item auto ... items>
			constexpr Variant<items ...> variant{};
		}

		template<Concept::is_item auto ... items>
		struct Implement::ValueInner<Variant<items ...>> final
		{
			std::variant<Value<decltype(items)>...> v{};

			template<Concept::is_string_identifier auto identifier>
			auto get_if() const noexcept
			{
				return std::get_if<Implement::index_of<identifier, items ...>>(v);
			}

			template<Concept::is_string_identifier auto identifier>
			auto get_if() noexcept
			{
				return std::get_if<Implement::index_of<identifier, items ...>>(v);
			}
		};
	}
}