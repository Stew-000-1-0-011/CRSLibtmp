#pragma once

#include <type_traits>
#include <utility>
#include <cstring>

#include "std_type.hpp"

namespace CRSLib
{
	namespace Implement
	{
		template<class T_>
		struct AlignedStorage_ final
		{
			static_assert(std::is_trivially_copyable<T_>::value, "This type is not trivially copyable, so cannot std::memcpy.");
			AlignedStorage_() = delete;
			AlignedStorage_(const AlignedStorage_&) = delete;
			AlignedStorage_(AlignedStorage_&&) = delete;

			using T = alignas(T_) unsigned char[sizeof(T_)];
		};
	}

	template<class T>
	using AlignedStorageT = typename Implement::AlignedStorage_<T>::T;

	// 引用: cpprefjp bit_cast
	template<typename To, typename From>
	To bit_cast(const From& from) noexcept
	{
		To result;
		std::memcpy(&result, &from, sizeof(To));
		return result;
	}
}