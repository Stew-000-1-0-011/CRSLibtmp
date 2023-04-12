#pragma once

#include <cmath>
#include <concepts>
#include <utility>

#include <CRSLibtmp/utility.hpp>
#include <CRSLibtmp/Motor.hpp>
#include <CRSLibtmp/Math.hpp>
#include <CRSLibtmp/Mechanism/gear.hpp>

namespace CRSLib::Mechanism
{
	namespace Implement
	{
		struct SteeringWheelMarker
		{};
	}

	template<class T>
	concept steering_wheel = std::derived_from<T, Implement::SteeringWheelMarker>;

	// 猿叫するか髭ダンスするかで髭ダンスを選んだ
	template<Mechanism::gear Gear_, Motor::speed_controlled_motor DrivingMotor>
	struct SteeringWheel final : Implement::SteeringWheelMarker
	{
		Gear steering;
		DrivingMotor driving;
		Math::Pose2D pose;
		double current_driving_speed{0};
		const double steer_coeffient;
		const double driving_coeffient;

		/**
		 * @brief Construct a new Steering Wheel object
		 * 
		 * @param steering 
		 * @param driving 
		 * @param pose 
		 * @param steer_revolution_ratio motor_revolution per angle
		 * @param wheel_radius 
		 * @param driving_revolution_ratio motor_revolution per angle
		 */
		SteeringWheel(cvref_same<Gear_> auto&& steering, cvref_same<DrivingMotor> auto&& driving, cvref_same<Math::Pose2D> auto&& pose, const double steer_revolution_ratio, const double wheel_radius, const double driving_revolution_ratio):
			steering{std::forward<decltype(steering)>(steering)},
			driving{std::forward<decltype(driving)>(driving)},
			pose{std::forward<decltype(pose)>(pose)},
			steer_coeffient{steer_revolution_ratio},
			driving_coeffient{driving_revolution_ratio / wheel_radius}
		{}

		void update(double steering_angle, const double driving_speed)
		{
			if(std::signbit(current_driving_speed) ^ std::signbit(driving_speed))
			{
				steering_angle += std::numbers::pi_v;
			}
			steering.update_angle(steering_angle * steer_coeffient);
			driving.update_speed(driving_speed * driving_coeffient);
			current_driving_speed = driving_speed;
		}
	};
}