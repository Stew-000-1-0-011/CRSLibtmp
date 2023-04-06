#pragma once

#include <array>

#include <CRSLibtmp/Motor.hpp>
#include <CRSLibtmp/std_type.hpp>

namespace CRSLib::Mechanism
{
	template<Motor::speed_controlled_motor M, u32 n>
	class OmniN final
	{
		std::array<M, n> motors{};
		
		public:
		OmniN()
	};
}