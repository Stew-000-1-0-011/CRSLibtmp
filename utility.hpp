/**
 * @file utility.hpp
 * @author Stew
 * @brief C++20で使うutility。CRSで使われるC++のバージョンに合わせられる
 * @version 0.1
 * @date 2023-03-07
 * 
 */

#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

#include "std_type.hpp"

namespace CRSLib
{
	template<class T, class U>
	concept cvref_same = std::same_as<std::remove_cvref_t<T>, U>;

	// 引用: cppref forward_like
	template<class T, class U>
	[[nodiscard]] constexpr auto&& forward_like(U&& x) noexcept
	{
		constexpr bool is_adding_const = std::is_const_v<std::remove_reference_t<T>>;
		if constexpr (std::is_lvalue_reference_v<T&&>) {
			if constexpr (is_adding_const) {
				return std::as_const(x);
			}
			else {
				return static_cast<U&>(x);
			}
		}
		else {
			if constexpr (is_adding_const) {
				return std::move(std::as_const(x));
			}
			else {
				return std::move(x);
			}
		}
	}

	namespace Implement
	{
		template<class T_>
		struct AlignedStorage_ final
		{
			static_assert(std::is_trivially_copyable<T_>::value, "This type is not trivially copyable, so cannot std::memcpy.");
			AlignedStorage_() = delete;
			AlignedStorage_(const AlignedStorage_&) = delete;
			AlignedStorage_(AlignedStorage_&&) = delete;

			using T = alignas(T_) byte[sizeof(T_)];
		};
	}

	template<class T>
	using AlignedStorageT = Implement::AlignedStorage_<T>::T;
}
