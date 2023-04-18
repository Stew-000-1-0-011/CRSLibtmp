/**
 * @file utility.hpp
 * @author Stew
 * @brief C++20で使うutility。CRSで使われるC++のバージョンに合わせられる
 * @version 0.1
 * @date 2023-03-07
 * 
 */

#pragma once

#include <cstdint>
#include <concepts>
#include <type_traits>
#include <utility>
#include <ratio>

#include "std_type.hpp"

namespace CRSLib
{
	template<class T, class U>
	concept cvref_same = std::same_as<std::remove_cvref_t<T>, U>;

	template<class T>
	concept non_cvref = std::same_as<std::remove_cvref_t<T>, T>;

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

	template<class Src, non_cvref Dest>
	using PropagateConst = std::conditional_t<std::is_const_v<Src>, const Dest, Dest>;

	template<class Src, non_cvref Dest>
	using PropagateVolatile = std::conditional_t<std::is_volatile_v<Src>, volatile Dest, Dest>;

	template<class Src, non_cvref Dest>
	using PropagateCV = PropagateConst<Src, PropagateVolatile<Src, Dest>>;

	template<class Src, non_cvref Dest>
	using PropagateLRef = std::conditional_t<std::is_lvalue_reference_v<Src>, Dest&, Dest>;

	template<class Src, non_cvref Dest>
	using PropagateRRef = std::conditional_t<std::is_rvalue_reference_v<Src>, Dest&&, Dest>;

	template<class Src, non_cvref Dest>
	using PropagateRef = PropagateLRef<Src, PropagateRRef<Src, Dest>>;

	template<class Src, non_cvref Dest>
	using PropagateCVRef = PropagateRef<Src, PropagateCV<std::remove_reference_t<Src>, Dest>>;

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

	template <class T>
	constexpr std::underlying_type_t<T> to_underlying(T value) noexcept
	{
		return static_cast<std::underlying_type_t<T>>(value);
	}

	inline constexpr auto ratio_get_numbers = []<std::intmax_t n, std::intmax_t d>(std::ratio<n, d>)
	{
		return std::pair{n, d};
	};

	template<std::floating_point T, std::intmax_t n, std::intmax_t d>
	constexpr auto ratio_to_floating(std::ratio<n, d>)
	{
		return static_cast<T>(n) / d;
	}

	template<class T>
	concept is_std_ratio = std::invocable<decltype(ratio_get_numbers), T>;

	template<class To, class From>
	To bit_cast(const From& from) noexcept
	{
		To result;
		std::memcpy(&result, &from, sizeof(To));
		return result;
	}
}
