#pragma once

#include <concepts>

namespace crs_lib::math::algebra::algebraic_structure {
	struct AlgebraicStructureMarker{};

	template<class T_>
	concept is_algebraic_structure = requires {
		requires std::derived_from<T_, AlgebraicStructureMarker>;
		typename T_::UnderlyingSet;
	};
}
