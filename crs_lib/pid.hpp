#pragma once

#include <crs_lib/math/algebra/ring.hpp>
#include <crs_lib/math/algebra/modul.hpp>

namespace crs_lib::pid::impl
{
	using math::algebra::modul::ops::operator*;
	using math::algebra::ring::ops::operator+;

	template<math::algebra::modul::is_linear_space T_>
	class Pid {
		using ValueT_ = typename T_::UnderlyingSet;
		using GainT_ = typename T_::Scalar::UnderlyingSet;

		GainT_ k_p;
		GainT_ k_i;
		GainT_ k_d;
		ValueT_ max_integral_error;
		ValueT_ integral_error;
		ValueT_ previous_error;

		public:
		static constexpr auto make(const GainT_& k_p, const GainT_& t_i, const GainT_& t_d, const ValueT_& max_integral_error) noexcept {
			return Pid{k_p, k_p * T_::Scalar::Multiplicative::inv(t_i), k_p * t_d, max_integral_error, };
		}

		T update(const ValueT_ error, const GainT_ dt) noexcept
		{
			const ValueT_ ret = k_p * error + k_i * integral_error + k_d * (error - previous_error) / dt;
			previous_error = error;
			integral_error += error * dt;
			
			if (integral_error > max_integral_error) {
				integral_error = max_integral_error;
			} else if (integral_error < -max_integral_error) {
				integral_error = -max_integral_error;
			}

			return ret;
		}
	};
}
