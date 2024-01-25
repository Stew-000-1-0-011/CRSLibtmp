#pragma once

#include <utility>
#include <concepts>

#include <CRSLibtmp/std_type.hpp>
#include <CRSLibtmp/Can/MainPC/pillarbox.hpp>

namespace CRSLib::Mechanism
{
	namespace Implement
	{
		struct SolenoidValveMarker
		{};
	}

	template<class T>
	concept solenoid_valve = std::derived_from<T, Implement::SolenoidValveMarker>;

	template<Can::MainPC::pillarbox CanPillarbox>
	struct SolenoidValve final : Implement::SolenoidValveMarker
	{
		CanPillarbox pillarbox;
		bool is_extend;
		bool open_equal_extend;

		SolenoidValve(CanPillarbox&& pillarbox, const bool initial_extendness = false, const bool open_equal_extend = true):
			pillarbox{std::move(pillarbox)},
			is_extend{initial_extendness},
			open_equal_extend{open_equal_extend}
		{}

		void extend()
		{
			is_extend = true;
			pillarbox.post(Can::DataField{.buffer={static_cast<byte>(open_equal_extend)}, .dlc=1});
		}

		void contract()
		{
			is_extend = false;
			pillarbox.post(Can::DataField{.buffer={static_cast<byte>(!open_equal_extend)}, .dlc=1});
		}

		void reverse()
		{
			if(is_extend)
			{
				contract();
			}
			else
			{
				extend();
			}
		}
	};
}
