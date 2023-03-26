#pragma once

namespace CRSLib::UnitCheck
{
	struct Isq final
	{
		int l, m, t, i, theta, n, j;

		static constexpr Isq zero_dimension(){return {0, 0, 0, 0, 0, 0, 0};}

		consteval auto operator!() const
		{
			return zero_dimension;
		}

		consteval auto operator+() const -> Isq
		{
			return *this;
		}

		consteval auto operator-() const -> Isq
		{
			return *this;
		}

		consteval friend auto operator*(const Isq& l, const Isq& r)
		{
			return {l.l + r.l, l.m + r.m, l.t + r.t, l.i + r.i, l.theta + r.theta, l.n + r.n, l.j + r.j};
		}

		consteval friend auto operator/(const Isq& l, const Isq& r)
		{
			return {l.l - r.l, l.m - r.m, l.t - r.t, l.i - r.i, l.theta - r.theta, l.n - r.n, l.j - r.j};
		}

		consteval friend auto operator<=>(const Isq& l, const Isq& r) = default;
		consteval friend bool operator==(const Isq& l, const Isq& r) = default;

		consteval friend auto operator+(const Isq& l, const Isq& r)
		{
			if(l != r)
			{
				constexpr int a = 0 / 0;
			}
			return l;
		}

		consteval friend auto operator-(const Isq& l, const Isq& r)
		{
			if(l != r)
			{
				constexpr int a = 0 / 0;
			}
			return l;
		}
	};
}