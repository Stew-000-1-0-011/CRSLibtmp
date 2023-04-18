#pragma once

#include <cstring>
#include <variant>
#include <algorithm>
#include <utility>
#include <optional>

#include <CRSLibtmp/std_type.hpp>
#include <CRSLibtmp/bit_operation.hpp>
#include <CRSLibtmp/utility.hpp>
#include <CRSLibtmp/Can/utility.hpp>

#include "register_map.hpp"
#include "filter_manager.hpp"
#include "utility.hpp"

namespace CRSLib::Can::Stm32::RM0008
{
	enum class TestMode : u8
	{
		Normal,
		LoopBack,
		Silent,
		SilentLoopBack
	};

	// デフォルトはAPB1が36MHzのとき、1Mbps
	struct BitTiming final
	{
		u16 prescaler{2};
		TestMode test_mode{TestMode::Normal};
		u8 resyncronization_jump_width{1};
		u8 time_segment_1{15};
		u8 time_segment_2{2};

		constexpr u32 calc_btr() const noexcept
		{
			return (prescaler - 1) << RegisterMap::BTR::shiftBRP
				| (to_underlying(test_mode) & 0x1 ? RegisterMap::BTR::LBKM : 0)
				| (to_underlying(test_mode) & 0x2 ? RegisterMap::BTR::SILM : 0)
				| (resyncronization_jump_width - 1) << RegisterMap::BTR::shiftSJW
				| (time_segment_1 - 1) << RegisterMap::BTR::shiftTS1
				| (time_segment_2 - 1) << RegisterMap::BTR::shiftTS2;
		}

		static constexpr u32 mask = RegisterMap::BTR::BRP | RegisterMap::BTR::LBKM | RegisterMap::BTR::SILM | RegisterMap::BTR::SJW | RegisterMap::BTR::TS1 | RegisterMap::BTR::TS2;

	};

	struct CanBusInit final
	{
		BitTiming bit_timing{};
		bool debug_freeze{true};
		bool time_triggered{false};
		bool auto_bus_off_recovery{true};
		bool auto_wake_up{false};
		// bool no_auto_retransmission{false};  /// @todo 未対応
		bool receive_fifo_locked{false};
		bool transmit_fifo_priority{false};

		void apply(CanRegister *const bxcan) const noexcept
		{
			BitOperation::change_masked_range(bxcan->BTR, BitTiming::mask, bit_timing.calc_btr());

			constexpr u32 mcr_mask = RegisterMap::MCR::DBF | RegisterMap::MCR::TTCM | RegisterMap::MCR::ABOM | RegisterMap::MCR::AWUM | RegisterMap::MCR::RFLM | RegisterMap::MCR::TXFP;
			const u32 new_mcr = (debug_freeze ? RegisterMap::MCR::DBF : 0) |
				(time_triggered ? RegisterMap::MCR::TTCM : 0) |
				(auto_bus_off_recovery ? RegisterMap::MCR::ABOM : 0) |
				(auto_wake_up ? RegisterMap::MCR::AWUM : 0) |
				// (no_auto_retransmission ? RegisterMap::MCR::NART : 0) |
				RegisterMap::MCR::NART |
				(receive_fifo_locked ? RegisterMap::MCR::RFLM : 0) |
				(transmit_fifo_priority ? RegisterMap::MCR::TXFP : 0);

			BitOperation::change_masked_range(bxcan->MCR, mcr_mask, new_mcr);
		}
	};

	struct ReceivedMessage final
	{
		DataField data;
		[[deprecated("Not Implemented.")]] u8 fmi;
		u16 time;
		u32 id;
	};

	class CanBus final
	{
		CanRegister * bxcan;

		public:
		CanBus(CanRegister *const bxcan, const CanBusInit& init = {}) noexcept:
			bxcan{bxcan}
		{
			enter_initialization();
			leave_sleep();

			init.apply(bxcan);

			leave_initialization();
		}

		~CanBus()
		{
			enter_sleep();
		}

		/// @brief メッセージを送信する。すでに送信バッファが埋まっていたら、新たに追加しようとしたものを含めた中で優先度の最も低いメッセージを破棄する。
		/// @attention 送信が成功したかどうかを監視しない。よってNonAutoRetransmissionモードは未対応。
		/// @param id 
		/// @param data 
		/// @return 破棄が発生したかどうか
		[[nodiscard]] bool post(const u32 id, const DataField& data) const noexcept
		{
			const u32 code = bxcan->TSR & RegisterMap::TSR::CODE >> RegisterMap::TSR::shiftCODE;
			constexpr u32 shift_tme = 26;
			
			bool ret = false;
			// 全部埋まっていたなら
			if(!(bxcan->TSR & (1 << (code + shift_tme))))
			{
				// もともと入っていたほうの優先順位が高いなら
				if(decode_mir(bxcan->TxMailBoxes[code].MIR) > id)
				{
					// 今回試みた送信を破棄する
					return false;
				}
				else
				{
					// そうでなければ送信をアボートする
					ret = abort_transmit(code);
				}
			}

			// 送信バッファに書き込む
			{
				// ID設定
				bxcan->TxMailBoxes[code].MIR = encode_mir(id);
				// DLC設定
				bxcan->TxMailBoxes[code].MDTR = data.dlc;
				// データ設定
				if(data.dlc <= 4)
				{
					u32 tmp;
					std::memcpy(&tmp, data.buffer, data.dlc);
					bxcan->TxMailBoxes[code].MDLR = tmp;
				}
				else
				{
					u32 tmp;
					std::memcpy(&tmp, data.buffer, 4);
					bxcan->TxMailBoxes[code].MDLR = tmp;
					std::memcpy(&tmp, data.buffer + 4, data.dlc - 4);
					bxcan->TxMailBoxes[code].MDHR = tmp;
				}
			}

			// 送信要求
			BitOperation::set_bit(bxcan->TxMailBoxes[code].MIR, RegisterMap::MIR::TXRQ);

			return ret;
		}

		[[nodiscard]] std::optional<ReceivedMessage> receive(const Fifo fifo) const noexcept
		{
			const auto fifo_ = to_underlying(fifo);

			// 受信バッファにデータがなければ
			if((bxcan->RFxR[fifo_] & RegisterMap::RFxR::FMP) >> RegisterMap::RFxR::shiftFMP == 0)
			{
				// 空を返す
				return std::nullopt;
			}

			// 受信バッファからデータを読み出す
			{
				const auto& mailbox = bxcan->FIFOMailBoxes[to_underlying(fifo)];
				ReceivedMessage msg;
				
				// CanDataFieldの取得
				msg.data.dlc = mailbox.MDTR & RegisterMap::MDTR::DLC >> RegisterMap::MDTR::shiftDLC;
				if(msg.data.dlc <= 4)
				{
					const u32 tmp = mailbox.MDLR;
					std::memcpy(msg.data.buffer, &tmp, msg.data.dlc);
				}
				else
				{
					u32 tmp = mailbox.MDLR;
					std::memcpy(msg.data.buffer, &tmp, 4);
					tmp = mailbox.MDHR;
					std::memcpy(msg.data.buffer + 4, &tmp, msg.data.dlc - 4);
				}

				// FMIの取得
				msg.fmi = mailbox.MDTR & RegisterMap::MDTR::FMI >> RegisterMap::MDTR::shiftFMI;

				// 受信時刻の取得
				msg.time = mailbox.MDTR & RegisterMap::MDTR::TIME >> RegisterMap::MDTR::shiftTIME;

				// IDの取得
				msg.id = decode_mir(mailbox.MIR);

				return {msg};
			}
		}

		private:
		[[nodiscard]] bool abort_transmit(const u32 code) const noexcept
		{
			BitOperation::set_bit(bxcan->TSR, RegisterMap::TSR::ABRQ0 << (code << 3));
			while(!BitOperation::is_bit_clear(bxcan->TSR, RegisterMap::TSR::ABRQ0 << (code << 3)));
			return BitOperation::is_bit_set(bxcan->TSR, RegisterMap::TSR::TXOK0 << (code << 3));
		}

		void leave_sleep() noexcept
		{
			BitOperation::clear_bit(bxcan->MCR, RegisterMap::MCR::SLEEP);
			while(!BitOperation::is_bit_clear(bxcan->MSR, RegisterMap::MSR::SLAK));
		}

		void enter_sleep() noexcept
		{
			BitOperation::set_bit(bxcan->MCR, RegisterMap::MCR::SLEEP);
			while(!BitOperation::is_bit_set(bxcan->MSR, RegisterMap::MSR::SLAK));
		}

		void enter_initialization() noexcept
		{
			BitOperation::set_bit(bxcan->MCR, RegisterMap::MCR::INRQ);
			while(!BitOperation::is_bit_set(bxcan->MSR, RegisterMap::MSR::INAK));
		}

		void leave_initialization() noexcept
		{
			BitOperation::clear_bit(bxcan->MCR, RegisterMap::MCR::INRQ);
			while(!BitOperation::is_bit_clear(bxcan->MSR, RegisterMap::MSR::INAK));
		}
	};
}
