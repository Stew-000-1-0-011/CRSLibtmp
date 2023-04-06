/**
 * @file marker.hpp
 * @author your name (you@domain.com)
 * @brief Motorのマーカー(マーカー... 継承させ、派生型がコレであることを派生型を書いたプログラマに明示させるための空の型)
 * @version 0.1
 * @date 2023-04-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include <concepts>

namespace CRSLib::Motor
{
	struct StateMarker
	{};

	template<class T>
	concept state = std::derived_from<T, StateMarker>;


	struct MotorMarker
	{};

	template<class T>
	concept motor = std::derived_from<T, MotorMarker> && requires
	{
		typename T::State;
		requires state<typename T::State>;
	};
}