#pragma once

#include <compare>
#include <concepts>

namespace CRSLib::UnitCheck
{
	struct QuantityMarker
	{
		friend bool operator==(const QuantityMarker&, const QuantityMarker&) = default;
	};

	template<class T>
	concept quantity = std::derived_from<T, QuantityMarker> && requires(const T l, const T r)
	{
		{l * r} noexcept;
		{l / r} noexcept;
	};

	template<quantity auto ... qs>
	struct Quantity final
	{
		friend consteval bool operator==(const Quantity&, const Quantity&){return true};
	};

	template<quantity auto ... qs_l, quantity auto ... qs_r>
	inline consteval auto operator*(Quantity<qs_l ...>, Quantity<qs_r ...>) noexcept -> Quantity<qs_l * qs_r ...>{return {}};

	template<quantity auto ... qs_l, quantity auto ... qs_r>
	inline consteval auto operator/(Quantity<qs_l ...>, Quantity<qs_r ...>) noexcept -> Quantity<qs_l / qs_r ...>{return {}};
}