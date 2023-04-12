#pragma once

#include "quantity.hpp"

namespace CRSLib::UnitCheck
{
	namespace Implement
	{
		struct UnitMarker
		{};
	}

	template<class T>
	concept unit = std::derived_from<T, Implement::UnitMarker>;

	template<auto m, quantity auto q>
	struct Unit final
	{};

	template<auto m_l, quantity auto q_l, auto m_r, quantity auto q_r>
	inline consteval auto operator*(Unit<m_l, q_l>, Unit<m_r, q_r>) noexcept -> Unit<m_l * m_r, q_l * q_r>{return {}};

	template<auto m_l, quantity auto q_l, auto m_r, quantity auto q_r>
	inline consteval auto operator/(Unit<m_l, q_l>, Unit<m_r, q_r>) noexcept -> Unit<m_l / m_r, q_l / q_r>{return {}};
}