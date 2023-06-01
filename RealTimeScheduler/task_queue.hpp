#pragma once

#include <optional>
#include <array>
#include <CRSLibtmp/std_type.hpp>
#include "task.hpp"

namespace CRSLib::RealTimeScheduler
{
	namespace Implement
	{
		struct TaskQueueMarker
		{};
	}

	template<class T>
	concept task_queue = std::derived_from<T, Implement::TaskQueueMarker> &&
	requires(T mut, const T imut)
	{
		{ imut.size() } -> std::convertible_to<size_t>;
		{ mut.push(Task{}) } -> std::convertible_to<bool>;
		{ mut.pop() } -> std::same_as<std::optional<Task>>;
	};

	/// @brief タスクキュー
	/// @attention 割り込み安全でも非同期安全でもない
	/// @tparam queue_size 
	template<size_t queue_size>
	class TaskQueue final : Implement::TaskQueueMarker
	{
		std::array<Task, queue_size> queue{};
		size_t head = 0;
		size_t tail = 0;

		public:
		auto size() const noexcept -> size_t
		{
			return tail - head;  // 負ならば符号なし整数の演算のため、勝手に % 2^(bit_width_of(size_t)) される
		}

		auto push(Task task) noexcept -> bool
		{
			if (size() == queue_size)
			{
				return false;
			}
			tail = increment(tail);
			queue[tail] = task;
			return true;
		}

		auto pop() noexcept -> std::optional<Task>
		{
			if (size() == 0)
			{
				return std::nullopt;
			}
			const auto tmp_head = head;
			head = increment(head);
			return queue[tmp_head];
		}

		private:
		static auto increment(size_t value) noexcept -> size_t
		{
			return (value + 1) % queue_size;
		}
	};
}
