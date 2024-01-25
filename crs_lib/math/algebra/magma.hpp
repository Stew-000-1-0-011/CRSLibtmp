#pragma once

#include <concepts>
#include <crs_lib/string_literal.hpp>
#include <crs_lib/operator_generator.hpp>
#include "algebraic_structure.hpp"

namespace crs_lib::math::algebra::magma {
	struct MagmaMarker : algebraic_structure::AlgebraicStructureMarker {};

	template<class T_>
	concept is_magma = algebraic_structure::is_algebraic_structure<T_> && requires {
		requires std::derived_from<T_, MagmaMarker>;
		requires requires(const T_::UnderlyingSet l, const T_::UnderlyingSet r) {
			{T_::op(l, r)} -> std::convertible_to<typename T_::UnderlyingSet>;
		};
	};

	struct CommutativeMagmaMarker : MagmaMarker {};

	template<class T_>
	concept is_commutative_magma = is_magma<T_> && requires {
		requires std::derived_from<T_, CommutativeMagmaMarker>;
	};

	template<is_magma T_, string_literal::StringLiteral op_symbol>
	struct MagmaOp : operator_generator::BinaryLeftOp <
		op_symbol
		, +[](const typename T_::UnderlyingSet& l, const typename T_::UnderlyingSet& r) {
			return T_::op(l, r);
		}
	> {};
	// こうしないと、opがラムダ式でない関数オブジェクトだったり、ジェネリックなラムダだったりしたときに
	// 変換すべき関数の型が推論できない。
}