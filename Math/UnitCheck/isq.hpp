#pragma once

#include <compare>

#include <CRSLibtmp/std_type.hpp>

#include "quantity.hpp"

namespace CRSLib::UnitCheck
{
	struct Isq final : QuantityMarker
	{
		i8 l{}, m{}, t{}, i{}, theta{}, n{}, j{};

		friend bool operator==(const Isq& l, const Isq& r) = default;

		friend constexpr Isq operator*(const Isq& l, const Isq& r) noexcept
		{
			return {{}, l.l + r.l, l.m + r.m, l.t + r.t, l.i + r.i, l.theta + r.theta, l.n + r.n, l.j + r.j};
		}

		friend constexpr Isq operator/(const Isq& l, const Isq& r) noexcept
		{
			return {{}, l.l - r.l, l.m - r.m, l.t - r.t, l.i - r.i, l.theta - r.theta, l.n - r.n, l.j - r.j};
		}
	};

	static_assert(quantity<Isq>);

	inline constexpr auto position = Isq{.l=1};
	inline constexpr auto speed = Isq{.l=1, .t=-1};
}