#pragma once

#include <CRSLibtmp/utility.hpp>
#include "unit.hpp"

namespace CRSLib::UnitCheck
{
	template<class T, unit auto u>
	struct United final
	{
		T v;
	};

	template<class TL, unit auto u_l, class TR, unit auto u_r>
	requires (u_l == u_r)
	inline constexpr auto operator+(cvref_same<United<TL, u_l>> auto&& l, cvref_same<United<TR, u_r>> auto&& r) -> United<decltype(l.v + r.v), u_l>
	{
		return {l.v + r.v};
	}

	template<class TL, unit auto u_l, class TR, unit auto u_r>
	requires (u_l == u_r)
	inline constexpr auto operator-(cvref_same<United<TL, u_l>> auto&& l, cvref_same<United<TR, u_r>> auto&& r) -> United<decltype(l.v - r.v), u_l>
	{
		return {l.v - r.v};
	}

	template<class TL, unit auto u_l, class TR, unit auto u_r>
	inline constexpr auto operator*(cvref_same<United<TL, u_l>> auto&& l, cvref_same<United<TR, u_r>> auto&& r) -> United<decltype(l.v * r.v), u_l * u_r>
	{
		return {l.v * r.v};
	}

	template<class TL, unit auto u_l, class TR, unit auto u_r>
	inline constexpr auto operator/(cvref_same<United<TL, u_l>> auto&& l, cvref_same<United<TR, u_r>> auto&& r) -> United<decltype(l.v / r.v), u_l / u_r>
	{
		return {l.v / r.v};
	}
}