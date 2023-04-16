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

	enum class Bitrate : u32
	{
		rate10k = 10000,
		rate20k = 20000,
		rate50k = 50000,
		rate100k = 100000,
		rate125k = 125000,
		rate250k = 250000,
		rate500k = 500000,
		rate1M = 1000000
	};

	inline constexpr auto max_ext_id = 0x1F'FF'FF'FF;
	inline constexpr auto max_std_id = 0x7FF;
}