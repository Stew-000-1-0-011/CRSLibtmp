#pragma once

#include <CRSLibtmp/std_type.hpp>
#include <CRSLibtmp/bit_operation.hpp>

#include "register_map.hpp"
#include "utility.hpp"

namespace CRSLib::Can::Stm32::RM0008
{
	struct FilterConfig final
	{
		bool is_list_mode{true};
		bool is_32bit{true};
		bool is_fifo1{false};
		u8 fmi{};

		static constexpr FilterConfig make_default(const Fifo fifo = Fifo::Fifo0) noexcept
		{
			return FilterConfig{.is_fifo1=(fifo == Fifo::Fifo1)};
		}
	};

	namespace FilterManager
	{
		/// @brief フィルタバンクの初期化を行う
		/// @todo FMIの設定
		/// @tparam n 
		/// @param can2sb 
		/// @param filters 
		template<u8 n>
		requires (n <= filter_bank_size)
		inline void initialize(const u8 can2sb, FilterConfig (& filters)[n]) noexcept
		{
			constexpr u32 filter_bank_mask = (1U << filter_bank_size) - 1U;
			{
				BitOperation::PinnedBit finit{filter_bank->FMaR, RegisterMap::FMaR::FINIT};

				BitOperation::set_bit(filter_bank->FMaR, can2sb << RegisterMap::FMaR::shiftCAN2SB);

				u32 is_list_mode = 0;
				u32 is_32bit = 0;
				u32 is_fifo1 = 0;

				{
					u8 i = 0;
					for(const auto filter : filters)
					{
						is_list_mode |= (filter.is_list_mode ? 1 : 0) << i;
						is_32bit |= (filter.is_32bit ? 1 : 0) << i;
						is_fifo1 |= (filter.is_fifo1 ? 1 : 0) << i;
						++i;
					}
				}

				BitOperation::change_masked_range(filter_bank->FMoR, filter_bank_mask, is_list_mode);
				BitOperation::change_masked_range(filter_bank->FSR, filter_bank_mask, is_32bit);
				BitOperation::change_masked_range(filter_bank->FFAR, filter_bank_mask, is_fifo1);
				BitOperation::change_masked_range(filter_bank->FAR, filter_bank_mask, 0x0U);
			}
		}

		void activate(const u8 index) noexcept
		{
			BitOperation::set_bit(filter_bank->FAR, 1U << index);
		}

		void deactivate(const u8 index) noexcept
		{
			BitOperation::clear_bit(filter_bank->FAR, 1U << index);
		}

		u32 make_list32(const u32 id, bool ide, bool rtr) noexcept
		{
			constexpr u32 ide_32 = 0x4;
			constexpr u32 rtr_32 = 0x2;
			constexpr u32 rshift_ext_id_32 = 11U - 3U;
			constexpr u32 lshift_std_id_32 = 21U;

			return (id & ~max_std_id & max_ext_id) >> rshift_ext_id_32 | // 拡張IDの上位18bit
				(id & max_std_id) << lshift_std_id_32 | // 標準IDの11bit
				ide ? ide_32 : 0 | // IDE
				rtr ? rtr_32 : 0; // RTR
		}

		Filter make_mask32(const u32 id, const u32 id_mask) noexcept
		{
			Filter ret;

			ret.FR1 = make_list32(id, id > max_std_id, false);
			ret.FR2 = make_list32(id_mask, true, true);

			return ret;
		}

		u32 make_list16(const u16 id, bool ide, bool rtr) noexcept
		{
			constexpr u32 ide_16 = 0x8;
			constexpr u32 rtr_16 = 0x10;
			constexpr u32 rshift_ext_id_16 = 11U + 18U - 3U;
			constexpr u32 lshift_std_id_16 = 5U;

			return (id & max_ext_id) >> rshift_ext_id_16 | // 拡張IDの上位3bit
				(id & max_std_id) << lshift_std_id_16 | // 標準IDの11bit
				ide ? ide_16 : 0 | // IDE
				rtr ? rtr_16 : 0; // RTR
		}

		u32 make_mask16(const u16 id, const u16 id_mask) noexcept
		{
			return make_list16(id, id > max_std_id, false) |
				make_list16(id_mask, true, true) << 16U;
		}

		[[nodiscard]] inline bool set_filter(const u8 index, const Filter& filter) noexcept
		{
			if(BitOperation::is_bit_clear(filter_bank->FAR, 1U << index))
			{
				filter_bank->filter_registers[index] = filter;
				return true;
			}
			else return false;
		}
	}
}
