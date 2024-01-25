#pragma once

#include <utility>

#include <CRSLibtmp/std_type.hpp>
#include <CRSLibtmp/utility.hpp>
#include <CRSLibtmp/Can/MainPC/pillarbox.hpp>

#include "basic_motor.hpp"

namespace CRSLib::Motor
{
	enum class C620State : u8
	{
		TurnOff,
		Stop,
		UpdateCurrent,
		AddCurrent,
		UpdateSpeed,
		AddSpeed,
		UpdatePosition,
		AddPosition
	};

	template<Can::MainPC::pillarbox Pillarbox>
	class C620 final : public MotorMarker
	{
		u8 id;
		Pillarbox pillar;

		C620(const u8 id, auto&& ... pillar_args):
			id{id},
			pillar{std::forward<decltype(pillar_args)>(pillar_args)...}
		{}
		
		[[deprecated]] void turn_off()
		{
			Can::DataField data{.buffer={id, to_underlying(C620State::TurnOff)}, .dlc=2};
			pillar.post(data);
		}

		void stop()
		{
			Can::DataField data{.buffer={id, to_underlying(C620State::Stop)}, .dlc=2};
			pillar.post(data);
		}

		void update_current(const float ampere)
		{
			Can::DataField data{.buffer={id, to_underlying(C620State::UpdateCurrent)}, .dlc=6};
			std::memcpy(data.buffer + 2, &ampere, sizeof(float));
			pillar.post(data);
		}

		void add_current(const float ampere)
		{
			Can::DataField data{.buffer={id, to_underlying(C620State::AddCurrent)}, .dlc=6};
			std::memcpy(data.buffer + 2, &ampere, sizeof(float));
			pillar.post(data);
		}

		void update_speed(const float rpm)
		{
			Can::DataField data{.buffer={id, to_underlying(C620State::UpdateSpeed)}, .dlc=6};
			std::memcpy(data.buffer + 2, &rpm, sizeof(float));
			pillar.post(data);
		}

		void add_speed(const float rpm)
		{
			Can::DataField data{.buffer={id, to_underlying(C620State::AddSpeed)}, .dlc=6};
			std::memcpy(data.buffer + 2, &rpm, sizeof(float));
			pillar.post(data);
		}

		void update_position(const float degree)
		{
			Can::DataField data{.buffer={id, to_underlying(C620State::UpdatePosition)}, .dlc=6};
			std::memcpy(data.buffer + 2, &degree, sizeof(float));
			pillar.post(data);
		}

		void add_position(const float degree)
		{
			Can::DataField data{.buffer={id, to_underlying(C620State::AddPosition)}, .dlc=6};
			std::memcpy(data.buffer + 2, &degree, sizeof(float));
			pillar.post(data);
		}
	};

}