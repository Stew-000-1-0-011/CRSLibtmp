#pragma once

#include <concepts>
#include <type_traits>

namespace crs_lib::bit_operation
{
	template<std::unsigned_integral T>
	inline constexpr void set_bit(T& value, const std::type_identity_t<T> bit) noexcept
	{
		value = value | bit;
	}

	template<std::unsigned_integral T>
	inline constexpr void clear_bit(T& value, const std::type_identity_t<T> bit) noexcept
	{
		value = value & ~bit;
	}

	template<std::unsigned_integral T>
	inline constexpr void toggle_bit(T& value, const std::type_identity_t<T> bit) noexcept
	{
		value = value ^ bit;
	}

	template<std::unsigned_integral T>
	[[nodiscard]] inline constexpr bool is_bit_set(const T value, const std::type_identity_t<T> bit) noexcept
	{
		return value & bit;
	}

	template<std::unsigned_integral T>
	[[nodiscard]] inline constexpr bool is_bit_clear(const T value, const std::type_identity_t<T> bit) noexcept
	{
		return !is_bit_set(value, bit);
	}

	template<std::unsigned_integral T>
	inline constexpr void change_masked_range(T& value, const std::type_identity_t<T> mask, const std::type_identity_t<T> new_value) noexcept
	{
		value = (value & ~mask) | (new_value & mask);
	}

	template<std::unsigned_integral T>
	struct PinnedBit final
	{
		T& value;
		const std::type_identity_t<T> bit;

		constexpr PinnedBit(T& value, const std::type_identity_t<T> bit) noexcept
			: value{value}, bit{bit}
		{
			set_bit(value, bit);
		}

		~PinnedBit() noexcept
		{
			clear_bit(value, bit);
		}
	};
}
