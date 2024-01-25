#pragma once

#include <cstddef>
#include <cstdint>
#if __cplusplus >= 202300L
#include <stdfloat>
#endif

namespace crs_lib::std_types {
#define cRsLib_DEFINE_STANDARD_INTEGER_TYPE(bit_size)\
	using i##bit_size = std::int##bit_size##_t;\
	using u##bit_size = std::uint##bit_size##_t

	cRsLib_DEFINE_STANDARD_INTEGER_TYPE(8);
	cRsLib_DEFINE_STANDARD_INTEGER_TYPE(16);
	cRsLib_DEFINE_STANDARD_INTEGER_TYPE(32);
	cRsLib_DEFINE_STANDARD_INTEGER_TYPE(64);

#undef cRsLib_DEFINE_STANDARD_INTEGER_TYPE

#if __cplusplus >= 202300L
	using f32 = std::float32_t;
	using f64 = std::float64_t;
#endif

	using i_ptr = std::intptr_t;
	using u_ptr = std::uintptr_t;
	using ptrdiff_t = std::ptrdiff_t;
	using usize = std::size_t;
	using nullptr_t = std::nullptr_t;
	using max_align_t = std::max_align_t;
	using byte = std::byte;
}

namespace crs_lib {
	using namespace std_types;
}
