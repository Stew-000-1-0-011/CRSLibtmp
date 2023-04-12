#pragma once

#include <utility>

#include <CRSLibtmp/std_type.hpp>
#include <CRSLibtmp/Can/MainPC/pillarbox.hpp>

#include "basic_motor.hpp"

namespace CRSLib::Motor
{
	template<Can::MainPC::pillarbox Pillarbox>
	class C620 final : MotorMarker
	{
		u8 id;
		Pillarbox pillar;

		C620(const u8 id, auto&& ... pillar_args):
			id{id},
			pillar{std::forward<decltype(pillar_args)>(pillar_args)...}
		{}
		
		[[deprecated]] void turn_off()
		{
			Can::DataField data{.buffer={id, TurnOff}, .dlc=2};
			pillar.post(data);
		}

		void stop()
		{
			Can::DataField data{.buffer={id, Stop}, .dlc=2};
			pillar.post(data);
		}

		void update_speed(const float rpm)
		{
			Can::DataField data{.buffer={id, Speed}, .dlc=6};
			std::memcpy(data.buffer + 2, &rpm, sizeof(float));
			pillar.post(data);
		}

		void update_position(const float degree)
		{
			Can::DataField data{.buffer={id, Speed}, .dlc=6};
			std::memcpy(data.buffer + 2, &degree, sizeof(float));
			pillar.post(data);
		}
	};

}