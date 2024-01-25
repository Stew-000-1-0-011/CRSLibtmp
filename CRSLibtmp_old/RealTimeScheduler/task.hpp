#pragma once

#include <type_traits>
#include <concepts>

namespace CRSLib::RealTimeScheduler
{
	class Task final
	{
		void * self = nullptr;
		void (*func)(void * self_) = nullptr;

		public:
		// 寿命を一切管理しない。lambdaが実行完了まで寿命を保つことを保証すること。
		static auto make(auto&& lambda) noexcept
		requires std::is_nothrow_invocable_v<decltype(lambda)>
		{
			return Task
			{
				.self = &lambda,
				.func = [](void * self_) { (*static_cast<std::remove_reference_t<decltype(lambda)> *>(self_))(); }
			};
		}

		auto operator()() const noexcept -> void
		{
			func(self);
		}
	};
}