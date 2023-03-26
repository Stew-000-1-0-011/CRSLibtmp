#pragma once

#include <shared_mutex>
#include <utility>
#include <type_traits>

#include <CRSLibtmp/utility.hpp>

namespace CRSLib
{
	template<class T, class Time>
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
		// TODO: forward_as_tupleを受け取るいい感じの引数
		template<class ... TimeArgs, class ... ValueArgs>
		MutexedLatest(std::tuple<ValueArgs ...>&& value, std::tuple<TimeArgs ...>&& time):
			MutexedLatest(std::move(value), std::move(time), std::index_sequence_for<ValueArgs ...>(), std::index_sequence_for<TimeArgs ...>())
		{}

		private:
		template<size_t ... time_indices, size_t ... value_indices>
		MutexedLatest(auto&& value, auto&& time, const std::index_sequence<value_indices ...>&, const std::index_sequence<time_indices ...>&):
			stamped{{std::get<value_indices>(value) ...}, {std::get<time_indices>(time) ...}}
		{}

		public:
		MutexedLatest(cvref_same<MutexedLatest> auto&& other):
			stamped{{forward_like<decltype(other)>(other.value)}, {forward_like<decltype(other)>(other.time)}}
		{}

		auto update(cvref_same<Stamped> auto&& maybe_new_stamped) -> bool
		{
			std::lock_guard lock{mutex};
			if(stamped.time <= maybe_new_stamped.time)
			{
				stamped = std::forward<decltype(maybe_new_stamped)>(maybe_new_stamped);
				return true;
			}
			return false;
		}

		auto get() const -> Stamped
		{
			std::shared_lock lock{mutex};
			return stamped;
		}
	};
}