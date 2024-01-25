#pragma once

#include <concepts>
#include "group.hpp"
#include "ring.hpp"

namespace crs_lib::math::algebra::modul {
	struct ModulMarker : group::LikeGroupMarker {};

	template<class T_>
	concept is_modul = group::is_abelian_group<T_> && requires {
		requires std::derived_from<T_, ModulMarker>;
		typename T_::Scalar;
		requires algebraic_structure::is_algebraic_structure<typename T_::Scalar>;
		requires requires(const typename T_::Scalar::UnderlyingSet l, const typename T_::UnderlyingSet r) {
			{T_::scalar_mul(l, r)} -> std::convertible_to<typename T_::UnderlyingSet>;
		};
	};

	template<class T_>
	concept is_ring_module = is_modul<T_> && requires {
		requires ring::is_ring<typename T_::Scalar>;
	};

	template<class T_>
	concept is_linear_space = is_ring_module<T_> && requires {
		typename T_::Scalar;
		requires ring::is_field<typename T_::Scalar>;
	};

	namespace ops {
		template<is_modul T_>
		inline constexpr auto operator*(const typename T_::Scalar::UnderlyingSet& l, const typename T_::UnderlyingSet& r) -> typename T_::UnderlyingSet {
			return T_::scalar_mul(l, r);
		}

		using ring::ops::operator+;
		using ring::ops::operator*;
	}
}
