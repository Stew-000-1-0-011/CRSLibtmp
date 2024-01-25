#pragma once

#include <CRSLibtmp/std_type.hpp>
#include <CRSLibtmp/Can/utility.hpp>
#include "config.hpp"
#include "register_map.hpp"

namespace CRSLib::Can::Stm32::RM0008
{
	#if Stew_is_connectivity_line
		inline constexpr u8 filter_bank_size = 28;
	#else
		inline constexpr u8 filter_bank_size = 14;
	#endif

	enum class Fifo : u8
	{
		Fifo0,
		Fifo1
	};

	[[nodiscard]] inline constexpr u32 decode_mir(const u32 mir) noexcept
	{
		constexpr u32 shift_exid = 3U;
		if(mir & RegisterMap::MIR::IDE)
		{
			return mir >> shift_exid;
		}
		else
		{
			return (mir & RegisterMap::MIR::STID_EXID) >> RegisterMap::MIR::shiftSTID_EXID;
		}
	}

	[[nodiscard]] inline constexpr u32 encode_mir(const u32 id) noexcept
	{
		constexpr u32 shift_exid = 3U;
		constexpr u32 shift_stid = 21U;
		// 拡張IDかどうか
		if(id > max_std_id)
		{
			return (id << shift_exid) | RegisterMap::MIR::IDE;
		}
		else
		{
			return (id << shift_stid);
		}
	}
}