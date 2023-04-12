#pragma once

#include <numbers>
#include <bit>
#include <atomic>
#include <cstring>
#include <memory>

#include <CRSLibtmp/std_type.hpp>
#include <CRSLibtmp/Can/MainPC/pillarbox.hpp>
#include <CRSLibtmp/Can/MainPC/letterbox.hpp>

#include "basic_motor.hpp"
#include "speed_controlled.hpp"
#include "position_controlled.hpp"
#include "angle_controlled.hpp"

namespace CRSLib::Motor
{
	template<Can::MainPC::pillarbox PillarBox>
	class C620 final : MotorMarker
	{
		struct State final
		{
			static constexpr auto angle_range = 8192;
			static constexpr auto current_range = 16384;

			u8 raw_data[7];

			constexpr float angle() const
			{
				return (raw_data[0] << 8 + raw_data[1]) * 360.0f / angle_range;
			}

			constexpr float speed() const
			{
				return (raw_data[2] << 8 + raw_data[3]);
			}

			constexpr float current() const
			{
				return std::bit_cast<i32>(raw_data[4] << (u32)8 + raw_data[5]) * 20.0f / current_range;
			}

			constexpr float temperature() const
			{
				return std::bit_cast<u8>(raw_data[6]);
			}
		};

		std::atomic<State> state{State{{}}};

		struct Callback final : Can::MainPC::Callback
		{
			std::atomic<State> *const state_p;

			void callback(const Can::DataField& data) override
			{
				State state;
				std::memcpy(state.raw_data, data.buffer, 7);
				*state_p = state;
			}
		};

		const std::shared_ptr<Callback> callback_sp{make_shared<Callback>(&(this->state))};
		Pillarbox pillar;

		C620():
		{

		}

		void stop()
		{

		}

		[[deprecated]] void turn_off()
		{}

		void update_speed(const float rpm)
		{
			
		}
	};


}