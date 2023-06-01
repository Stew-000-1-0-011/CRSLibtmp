#pragma once

#include <concepts>
#include <tuple>
#include <CRSLibtmp/std_type.hpp>
#include "task_queue.hpp"
#include "watch_dog_timer.hpp"

namespace CRSLib::RealTimeScheduler
{
	// 欲を言えば、schedulerにwatch_dog_timerを持たせ、それをもとに各関数の呼び出し周期の上界を保証し、
	// そこから各タスクに時間超過によるエラーが発生しないことを保証したい
	template<watch_dog_timer WatchDogTimer, class IndexEnum, task_queue ... TaskQueues>
	requires std::is_enum_v<IndexEnum> && std::unsigned_integral<std::underlying_type_t<IndexEnum>>
	class Scheduler final
	{
		std::tuple<TaskQueues ...> task_queues{};
		WatchDogTimer watch_dog_timer;

		public:
		Scheduler() noexcept
			: control_period(control_period)
		{}

		template<IndexEnum index>
		auto push(Task task) noexcept -> bool
		{
			return std::get<static_cast<size_t>(index)>(task_queues).push(task);
		}

		void run_once() noexcept
		{
			
		}
	};
}