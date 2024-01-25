#pragma once

#include <type_traits>

namespace CRSLib::Math
{
	template<class T>
	requires std::is_arithmetic_v<T>
	class Pid
	{
		T p_gain;
		T i_gain;
		T d_gain;

		T previous;
		T integral;

		public:
		constexpr Pid(const T p_gain, const T i_gain, const T d_gain, const T previous = 0, const T integral = 0) noexcept
			:
			p_gain(p_gain),
			i_gain(i_gain),
			d_gain(d_gain),
			previous(previous),
			integral(integral)
		{}

		T update(const T target, const T current) noexcept
		{
			const T error = target - current;
			const T p = p_gain * error;
			integral += error;
			const T i = i_gain * integral;
			const T d = d_gain * (current - previous);
			previous = current;
			return p + i + d;
		}
	};
}
