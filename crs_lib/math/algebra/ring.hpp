#pragma once

#include <concepts>
#include <crs_lib/string_literal.hpp>
#include <crs_lib/operator_generator.hpp>
#include "algebraic_structure.hpp"
#include "magma.hpp"
#include "group.hpp"

namespace crs_lib::math::algebra::ring {
	struct RngMarker : algebraic_structure::AlgebraicStructureMarker {};

	template<class T_>
	concept is_rng = algebraic_structure::is_algebraic_structure<T_> && requires {
		requires std::derived_from<T_, RngMarker>;

		typename T_::Additive;
		requires group::is_abelian_group<typename T_::Additive>;
		typename T_::Multiplicative;
		requires group::is_semigroup<typename T_::Multiplicative>;
	};

	template<class T_>
	concept is_ring = is_rng<T_> && requires {
		requires group::is_monoid<typename T_::Multiplicative>;
	};

	template<class T_>
	concept is_skew_field = is_ring<T_> && requires {
		requires group::is_group<typename T_::Multiplicative>;
	};

	template<class T_>
	concept is_field = is_skew_field<T_> && requires {
		requires magma::is_commutative_magma<typename T_::Multiplicative>;
	};

	template<is_rng T_, string_literal::StringLiteral add_op_symbol_, string_literal::StringLiteral sub_op_symbol_, string_literal::StringLiteral mul_op_symbol_>
	struct RngOp : operator_generator::BinaryLeftOp <
		add_op_symbol_
		, +[](const typename T_::UnderlyingSet& l, const typename T_::UnderlyingSet& r) {
			return T_::Additive::op(l, r);
		}
	>
	, operator_generator::BinaryLeftOp <
		sub_op_symbol_
		, +[](const typename T_::UnderlyingSet& l, const typename T_::UnderlyingSet& r) {
			return T_::Additive::op(l, T_::Additive::inv(r));
		}
	>
	, operator_generator::BinaryLeftOp <
		mul_op_symbol_
		, +[](const typename T_::UnderlyingSet& l, const typename T_::UnderlyingSet& r) {
			return T_::Multiplicative::op(l, r);
		}
	> {};

	template<is_ring T_, string_literal::StringLiteral add_op_symbol_, string_literal::StringLiteral sub_op_symbol_, string_literal::StringLiteral mul_op_symbol_, string_literal::StringLiteral div_op_symbol_>
	struct RingOp : operator_generator::BinaryLeftOp <
		add_op_symbol_
		, +[](const typename T_::UnderlyingSet& l, const typename T_::UnderlyingSet& r) {
			return T_::Additive::op(l, r);
		}
	>
	, operator_generator::BinaryLeftOp <
		sub_op_symbol_
		, +[](const typename T_::UnderlyingSet& l, const typename T_::UnderlyingSet& r) {
			return T_::Additive::op(l, T_::Additive::inv(r));
		}
	>
	, operator_generator::BinaryLeftOp <
		mul_op_symbol_
		, +[](const typename T_::UnderlyingSet& l, const typename T_::UnderlyingSet& r) {
			return T_::Multiplicative::op(l, r);
		}
	>
	, operator_generator::BinaryLeftOp <
		div_op_symbol_
		, +[](const typename T_::UnderlyingSet& l, const typename T_::UnderlyingSet& r) {
			return T_::Multiplicative::op(l, T_::Multiplicative::inv(r));
		}
	> {};

	template<is_skew_field T_, >

	// template<is_rng T_>
	// struct RingOps final {
	// 	static constexpr auto add(const typename T_::UnderlyingSet& l, const typename T_::UnderlyingSet& r) -> typename T_::UnderlyingSet {
	// 		using group::ops::operator+;
	// 		using group::ops::operator-;
	// 	}

	// 	static constexpr auto mul(const typename T_::UnderlyingSet& l, const typename T_::UnderlyingSet& r) -> typename T_::UnderlyingSet {
	// 		return T_::Multiplicative::op(l, r);
	// 	}
	// };

	// template<is_skew_field T_>
	// struct FieldOps final {
	// 	static constexpr auto add(const typename T_::UnderlyingSet& l, const typename T_::UnderlyingSet& r) -> typename T_::UnderlyingSet {
	// 		return T_::Additive::op(l, r);
	// 	}

	// 	static constexpr auto mul(const typename T_::UnderlyingSet& l, const typename T_::UnderlyingSet& r) -> typename T_::UnderlyingSet {
	// 		return T_::Multiplicative::op(l, r);
	// 	}

	// 	static constexpr auto inv(const typename T_::UnderlyingSet& x) -> typename T_::UnderlyingSet {
	// 		return T_::Multiplicative::inv(x);
	// 	}
	// };
	// };
}
