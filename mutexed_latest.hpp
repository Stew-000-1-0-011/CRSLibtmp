#pragma once

#include <shared_mutex>
#include <utility>
#include <type_traits>
#include <concepts>

#include <CRSLibtmp/utility.hpp>

namespace CRSLib
{
	/**
	 * @brief 
	 * T型のデータが並列に書き込み要求を受けうる場合、その中からある意味で一番最新のもので更新を行うためのクラス。
	 * 内部で(shared_)mutexを用いている。
	 * @tparam T 格納されるデータ型
	 * @tparam Time 更新するかどうかの基準となる時刻を意味する型
	 */
	template<class T, class Time>
	requires requires(const Time& t1, const Time& t2)
		{
			{t1 < t2} -> std::same_as<bool>;
			requires std::copy_constructible<Time>;
			requires std::copy_constructible<T>;
		}
	class MutexedLatest final
	{
		public:
		struct Stamped final
		{
			T value;
			Time time;
		};
		
		private:
		Stamped stamped;
		mutable std::shared_mutex mutex{};

		public:
		/**
		 * @brief Construct a new Mutexed Latest object
		 * 
		 * @tparam ValueArgs 下で説明
		 * @tparam TimeArgs 下で説明
		 * @param value forward_as_tupleされたtimeの引数
		 * @param time forward_as_tupleされたtimeの引数
		 */
		template<class ... ValueArgs, class ... TimeArgs>
		MutexedLatest(std::tuple<ValueArgs ...>&& value, std::tuple<TimeArgs ...>&& time):
			MutexedLatest(std::move(value), std::move(time), std::index_sequence_for<ValueArgs ...>(), std::index_sequence_for<TimeArgs ...>())
		{}

		private:
		template<size_t ... time_indices, size_t ... value_indices>
		MutexedLatest(auto&& value, auto&& time, const std::index_sequence<value_indices ...>&, const std::index_sequence<time_indices ...>&):
			stamped{{std::forward<decltype(std::get<value_indices>(value))>(std::get<value_indices>(value)) ...}, {std::forward<decltype(std::get<time_indices>(time))>(std::get<time_indices>(time)) ...}}
		{}

		public:
		/**
		 * @brief Construct a new Mutexed Latest object
		 * Copy/Move Constructor.
		 * @param other 
		 */
		MutexedLatest(cvref_same<MutexedLatest> auto&& other):
			stamped{{forward_like<decltype(other)>(other.value)}, {forward_like<decltype(other)>(other.time)}}
		{}

		/**
		 * @brief 
		 * 時刻つきのデータで更新を試みる
		 * @tparam update_if_same_period 同時刻のとき更新するかどうか
		 * @param maybe_new_stamped 更新を試みるデータ
		 * @return true 渡されたデータが新しいか同時刻のものであり、更新に成功した
		 * @return false 渡されたデータが古く、更新しなかった
		 */
		template<bool update_if_same_period = true>
		auto update(cvref_same<Stamped> auto&& maybe_new_stamped) -> bool
		{
			std::lock_guard lock{mutex};

			if(stamped.time < maybe_new_stamped.time)
			{
				stamped = std::forward<decltype(maybe_new_stamped)>(maybe_new_stamped);
				return true;
			}
			else
			{
				if constexpr(update_if_same_period)
				{
					if(stamped.time == maybe_new_stamped.time)
					{
						stamped = std::forward<decltype(maybe_new_stamped)>(maybe_new_stamped);
						return true;
					}
				}
			}

			return false;
		}

		/**
		 * @brief 
		 * データを時刻つきで得る。
		 * @return Stamped 
		 */
		auto get() const -> Stamped
		{
			std::shared_lock lock{mutex};
			return stamped;
		}
	};
}