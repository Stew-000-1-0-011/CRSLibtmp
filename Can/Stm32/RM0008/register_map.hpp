#pragma once

#include <CRSLibtmp/std_type.hpp>
#include "config.hpp"

namespace CRSLib::Can::Stm32::RM0008
{
	struct Mailbox final
	{
		volatile u32 MIR;
		volatile u32 MDTR;
		volatile u32 MDLR;
		volatile u32 MDHR;
	};

	static_assert(sizeof(Mailbox) == 16);

	struct CanRegister final
	{
		volatile u32 MCR;
		volatile u32 MSR;
		volatile u32 TSR;
		volatile u32 RFxR[2];
		volatile u32 IER;
		volatile u32 ESR;
		volatile u32 BTR;

		u32 reserved0[88];
		
		Mailbox TxMailBoxes[3];
		Mailbox FIFOMailBoxes[2];
		
		u32 reserved1[12];
	};

	static_assert(sizeof(CanRegister) == 128 * 4);

	struct Filter final
	{
		volatile u32 FR1;
		volatile u32 FR2;
	};

	static_assert(sizeof(Filter) == 8);

	struct FilterRegister final
	{
		volatile u32 FMaR;
		volatile u32 FMoR;
		
		u32 reserved2;
		
		volatile u32 FSR;
		
		u32 reserved3;
		
		volatile u32 FFAR;
		
		u32 reserved4;
		
		volatile u32 FAR;
		
		u32 reserved5[8];
		
		Filter filter_registers[14];
	};

	static_assert(sizeof(FilterRegister) == 44 * 4);

	// リンク時定数が欲しい...
	inline CanRegister *const can1 = reinterpret_cast<CanRegister *>(static_cast<u_ptr>(0x4000'6400U));
	inline FilterRegister *const filter_bank = reinterpret_cast<FilterRegister *>(static_cast<u_ptr>(0x4000'6600U));
	#if Stew_is_connectivity_line
		inline CanRegister *const can2 = reinterpret_cast<CanRegister *>(static_cast<u_ptr>(0x4000'6800U));
	#endif

	namespace RegisterMap
	{
		// CAN control and status registers

		namespace MCR // Master Control Register
		{
			// reserved
			inline constexpr u32 DBF = 0x00'01'00'00U; // Debug Freeze
			inline constexpr u32 RESET = 0x00'00'80'00U; // bxCAN software master reset
			// reserved
			inline constexpr u32 TTCM = 0x00'00'00'80U; // Time triggered communication mode
			inline constexpr u32 ABOM = 0x00'00'00'40U; // Automatic bus-off management
			inline constexpr u32 AWUM = 0x00'00'00'20U; // Automatic wakeup mode
			inline constexpr u32 NART = 0x00'00'00'10U; // No automatic retransmission
			inline constexpr u32 RFLM = 0x00'00'00'08U; // Receive FIFO locked mode
			inline constexpr u32 TXFP = 0x00'00'00'04U; // Transmit FIFO priority
			inline constexpr u32 SLEEP = 0x00'00'00'02U; // Sleep mode request
			inline constexpr u32 INRQ = 0x00'00'00'01U; // Initialization request
		}

		namespace MSR // Master Status Register
		{
			// reserved
			inline constexpr u32 RX = 0x00'00'08'00U; // CAN Rx signal
			inline constexpr u32 SAMP = 0x00'00'04'00U; // Last sample point
			inline constexpr u32 RXM = 0x00'00'02'00U; // Receive mode
			inline constexpr u32 TXM = 0x00'00'01'00U; // Transmit mode
			// reserved
			inline constexpr u32 SLAKI = 0x00'00'00'10U; // Sleep acknowledge interrupt
			inline constexpr u32 WKUI = 0x00'00'00'08U; // Wakeup interrupt
			inline constexpr u32 ERRI = 0x00'00'00'04U; // Error interrupt
			inline constexpr u32 SLAK = 0x00'00'00'02U; // Sleep acknowledge
			inline constexpr u32 INAK = 0x00'00'00'01U; // Initialization acknowledge
		}

		namespace TSR // Transmit Status Register
		{
			inline constexpr u32 LOW2 = 0x80'00'00'00U; // Lowest priority flag for mailbox 2
			inline constexpr u32 LOW1 = 0x40'00'00'00U; // Lowest priority flag for mailbox 1
			inline constexpr u32 LOW0 = 0x20'00'00'00U; // Lowest priority flag for mailbox 0
			inline constexpr u32 TME2 = 0x10'00'00'00U; // Transmit mailbox 2 empty
			inline constexpr u32 TME1 = 0x08'00'00'00U; // Transmit mailbox 1 empty
			inline constexpr u32 TME0 = 0x04'00'00'00U; // Transmit mailbox 0 empty
			inline constexpr u32 CODE = 0x03'00'00'00U; // Mailbox code
			inline constexpr u32 shiftCODE = 24U;
			inline constexpr u32 ABRQ2 = 0x00'80'00'00U; // Abort request for mailbox 2
			// reserved
			inline constexpr u32 TERR2 = 0x00'08'00'00U; // Transmission error of mailbox 2
			inline constexpr u32 ALST2 = 0x00'04'00'00U; // Arbitration lost for mailbox 2
			inline constexpr u32 TXOK2 = 0x00'02'00'00U; // Transmission OK of mailbox 2
			inline constexpr u32 RQCP2 = 0x00'01'00'00U; // Request completed mailbox 2
			inline constexpr u32 ABRQ1 = 0x00'00'80'00U; // Abort request for mailbox 1
			// reserved
			inline constexpr u32 TERR1 = 0x00'00'08'00U; // Transmission error of mailbox 1
			inline constexpr u32 ALST1 = 0x00'00'04'00U; // Arbitration lost for mailbox 1
			inline constexpr u32 TXOK1 = 0x00'00'02'00U; // Transmission OK of mailbox 1
			inline constexpr u32 RQCP1 = 0x00'00'01'00U; // Request completed mailbox 1
			inline constexpr u32 ABRQ0 = 0x00'00'00'80U; // Abort request for mailbox 0
			// reserved
			inline constexpr u32 TERR0 = 0x00'00'00'08U; // Transmission error of mailbox 0
			inline constexpr u32 ALST0 = 0x00'00'00'04U; // Arbitration lost for mailbox 0
			inline constexpr u32 TXOK0 = 0x00'00'00'02U; // Transmission OK of mailbox 0
			inline constexpr u32 RQCP0 = 0x00'00'00'01U; // Request completed mailbox 0
		}

		namespace RFxR // Receive FIFO x Register
		{
			// reserved
			inline constexpr u32 RFOM = 0x00'00'00'20U; // Release FIFO x output mailbox
			inline constexpr u32 FOVR = 0x00'00'00'10U; // FIFO x overrun
			inline constexpr u32 FULL = 0x00'00'00'08U; // FIFO x full
			// reserved
			inline constexpr u32 FMP = 0x00'00'00'03U; // FIFO x message pending
			inline constexpr u32 shiftFMP = 0U;
		}

		namespace IER // Interrupt Enable Register
		{
			// reserved
			inline constexpr u32 SLKIE = 0x00'02'00'00U; // Sleep interrupt enable
			inline constexpr u32 WKUIE = 0x00'01'00'00U; // Wakeup interrupt enable
			inline constexpr u32 ERRIE = 0x00'00'80'00U; // Error interrupt enable
			// reserved
			inline constexpr u32 LECIE = 0x00'00'08'00U; // Last error code interrupt enable
			inline constexpr u32 BOFIE = 0x00'00'04'00U; // Bus-off interrupt enable
			inline constexpr u32 EPVIE = 0x00'00'02'00U; // Error passive interrupt enable
			inline constexpr u32 EWGIE = 0x00'00'01'00U; // Error warning interrupt enable
			// reserved
			inline constexpr u32 FOVIE1 = 0x00'00'00'40U; // FIFO 1 overrun interrupt enable
			inline constexpr u32 FFIE1 = 0x00'00'00'20U; // FIFO 1 full interrupt enable
			inline constexpr u32 FMPIE1 = 0x00'00'00'10U; // FIFO 1 message pending interrupt enable
			inline constexpr u32 FOVIE0 = 0x00'00'00'08U; // FIFO 0 overrun interrupt enable
			inline constexpr u32 FFIE0 = 0x00'00'00'04U; // FIFO 0 full interrupt enable
			inline constexpr u32 FMPIE0 = 0x00'00'00'02U; // FIFO 0 message pending interrupt enable
			inline constexpr u32 TMEIE = 0x00'00'00'01U; // Transmit mailbox empty interrupt enable
		}

		namespace ESR // Error Status Register
		{
			// reserved
			inline constexpr u32 REC = 0xFF'00'00'00U; // Receive error counter
			inline constexpr u32 shiftREC = 24U;
			inline constexpr u32 TEC = 0x00'FF'00'00U; // Transmit error counter
			inline constexpr u32 shiftTEC = 16U;
			// reserved
			inline constexpr u32 LEC = 0x00'00'00'70U; // Last error code
			inline constexpr u32 shiftLEC = 4U;
			// reserved
			inline constexpr u32 BOFF = 0x00'00'00'04U; // Bus-off flag
			inline constexpr u32 EPVF = 0x00'00'00'02U; // Error passive flag
			inline constexpr u32 EWGF = 0x00'00'00'01U; // Error warning flag
		}

		namespace BTR // Bit Timing Register
		{
			inline constexpr u32 SILM = 0x80'00'00'00U; // Silent mode (debug)
			inline constexpr u32 LBKM = 0x40'00'00'00U; // Loopback mode (debug)
			// reserved
			inline constexpr u32 SJW = 0x03'00'00'00U; // Resynchronization jump width
			inline constexpr u32 shiftSJW = 24U;
			// reserved
			inline constexpr u32 TS2 = 0x00'70'00'00U; // Time segment 2
			inline constexpr u32 shiftTS2 = 20U;
			inline constexpr u32 TS1 = 0x00'0F'00'00U; // Time segment 1
			inline constexpr u32 shiftTS1 = 16U;
			// reserved
			inline constexpr u32 BRP = 0x00'00'03'FFU; // Baud rate prescaler
			inline constexpr u32 shiftBRP = 0U;
		}


		// CAN mailbox registers

		// Transmit and receive mailboxes have the same registers except:
		// - A receive mailbox is always read-only
		// - A transmit mailbox is weite-enabled only when empty, corresponding TME bit in the TSR register is set

		namespace MIR // Mailbox identifier register
		{
			inline constexpr u32 STID_EXID = 0xFF'E0'00'00U; // Standard identifier or extended identifier
			inline constexpr u32 shiftSTID_EXID = 21U;
			inline constexpr u32 EXID_LOW = 0x00'1F'FF'F8U; // Extended identifier low bits
			inline constexpr u32 shiftEXID_LOW = 3U;
			inline constexpr u32 IDE = 0x00'00'00'04U; // Identifier extension
			inline constexpr u32 RTR = 0x00'00'00'02U; // Remote transmission request
			inline constexpr u32 TXRQ = 0x00'00'00'01U; // Transmit mailbox request (Transmit mailbox only)
		}

		namespace MDTR // Mailbox data length control and time stamp register
		{
			inline constexpr u32 TIME = 0xFF'FF'00'00U; // Message time stamp
			inline constexpr u32 shiftTIME = 16U;
			inline constexpr u32 FMI = 0x00'00'FF'00U; // Filter match index (Receive mailbox only)
			inline constexpr u32 shiftFMI = 8U;
			inline constexpr u32 TGT = 0x00'00'01'00U; // Transmit global time (Transmit mailbox only)
			// reserved
			inline constexpr u32 DLC = 0x00'00'00'0FU; // Data length code
			inline constexpr u32 shiftDLC = 0U;
		}

		// CAN filter registers
		// except for FiR and FAR, all filter registers cannot be written without setting FINIT bit in the FMR register.
		// FiRX can be written even when the filter is disabled (FACTi bit in the FAxR register is cleared).

		namespace FMaR // Filter master register
		{
			// reserved
			inline constexpr u32 CAN2SB = 0x00'00'FF'00U; // CAN2 start bank
			inline constexpr u32 shiftCAN2SB = 8U;
			// reserved
			inline constexpr u32 FINIT = 0x00'00'00'01U; // Filter init mode
		}
	}
}