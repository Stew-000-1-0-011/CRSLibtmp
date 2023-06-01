#pragma once

namespace CRSLib::RealTimeScheduler
{
	template<class T>
	concept watch_dog_timer = requires(T mut, const T imut)
	{
		{imut.is_expired()} noexcept -> std::convertible_to<bool>;
		{mut.reset()} noexcept;
		{mut.bark()} noexcept;
	};
}