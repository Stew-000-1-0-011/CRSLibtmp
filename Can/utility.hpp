#pragma once

#include <CRSLibtmp/utility.hpp>
#include <CRSLibtmp/std_type.hpp>

namespace CRSLib::Can
{
	struct DataField final
	{
		static constexpr auto can_mtu = 8;
		byte buffer[can_mtu]{};
		u8 dlc{};
	};

	

	inline constexpr auto max_ext_id = 0x03'FF'FF;
}