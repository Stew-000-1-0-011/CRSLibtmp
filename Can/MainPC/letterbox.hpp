#pragma once

#include <vector>
#include <memory>

#include <CRSLibtmp/std_type.hpp>

#include <CRSLibtmp/Can/utility.hpp>

namespace CRSLib::Can::MainPC
{
	struct Callback
	{
		virtual ~Callback() = default;
		virtual void callback(const DataField& data) = 0;
	};

	template<class T>
	concept letterbox = std::move_constructible<T> && requires(T mut, const T imut, const std::shared_ptr<Callback> callback, const DataField data)
	{
		mut.add(callback);
	};

	struct LetterboxOfCluster final
	{
		u32 id{max_ext_id};
		std::vector<std::weak_ptr<Callback>> callbacks{};

		void add(const std::shared_ptr<Callback>& callback)
		{
			callbacks.emplace_back(callback);
		}

		void receive(const DataField& data)
		{
			for(const auto& callback : callbacks)
			{
				if(const auto sp = callback.lock(); sp)
				{
					sp->callback(data);
				}
			}
		}
	};

	// can_tx用
	template<class T>
	concept cluster_letterbox = std::move_constructible<T> && requires(T mut, const T imut, LetterboxOfCluster box, const u32 id, const DataField data)
	{
		mut.add(box);
		mut.build();
		imut.receive(id, data);
	};
}