#pragma once

#include <concepts>
#include <optional>
#include <crs_lib/string_literal.hpp>
#include "magma.hpp"

namespace crs_lib::math::algebra::group {
	struct LikeGroupMarker : magma::MagmaMarker {
		virtual consteval auto is_associative() const noexcept -> std::optional<bool> {
			return std::nullopt;
		}
	};

	template<class T_>
	concept is_like_group = magma::is_magma<T_> && requires {
		requires std::derived_from<T_, LikeGroupMarker>;
	};

	template<class T_>
	concept has_identity = is_like_group<T_> && requires {
		requires requires {
			{T_::id()} -> std::convertible_to<typename T_::UnderlyingSet>;
		};
	};

	template<class T_>
	concept has_inverse = is_like_group<T_> && requires {
		requires requires(const typename T_::UnderlyingSet x) {
			{T_::inv(x)} -> std::convertible_to<typename T_::UnderlyingSet>;
		};
	};

	template<class T_>
	concept has_associativity = is_like_group<T_> && requires {
		requires T_::is_associative();
	};

	template<class T_>
	concept is_semigroup = is_like_group<T_> && requires {
		requires has_associativity<T_>;
	};

	template<class T_>
	concept is_unital_magma = is_like_group<T_> && requires {
		requires has_identity<T_>;
	};

	template<class T_>
	concept is_quasigroup = is_like_group<T_> && requires {
		requires has_inverse<T_>;
	};

	template<class T_>
	concept is_loop = is_like_group<T_> && requires {
		requires has_identity<T_>;
		requires has_inverse<T_>;
	};

	template<class T_>
	concept is_associative_quasigroup = is_like_group<T_> && requires {
		requires has_associativity<T_>;
		requires has_inverse<T_>;
	};

	template<class T_>
	concept is_monoid = is_like_group<T_> && requires {
		requires has_associativity<T_>;
		requires has_identity<T_>;
	};

	template<class T_>
	concept is_group = is_like_group<T_> && requires {
		requires has_associativity<T_>;
		requires has_identity<T_>;
		requires has_inverse<T_>;
	};

	template<class T_>
	concept is_abelian_group = is_group<T_> && requires {
		requires magma::is_commutative_magma<T_>;
	};

	template<is_quasigroup T_, string_literal::StringLiteral op_symbol_, string_literal::StringLiteral sub_op_symbol_, string_literal::StringLiteral negate_op_symbol_>
	struct GroupOp
	: operator_generator::BinaryLeftOp <
		op_symbol_
		, +[](const typename T_::UnderlyingSet& l, const typename T_::UnderlyingSet& r) {
			return T_::op(l, r);
		}
	>
	, operator_generator::UnaryOp <
		negate_op_symbol_
		, +[](const typename T_::UnderlyingSet& x) {
			return T_::inv(x);
		}
	>
	, operator_generator::BinaryLeftOp <
		sub_op_symbol_
		, +[](const typename T_::UnderlyingSet& l, const typename T_::UnderlyingSet& r) {
			return T_::op(l, T_::inv(r));
		}
	> {};
}