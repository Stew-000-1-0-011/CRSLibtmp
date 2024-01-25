#pragma once

#include <vector>
#include <memory>

#include <CRSLibtmp/std_type.hpp>

#include <CRSLibtmp/Can/utility.hpp>

namespace CRSLib::Can::MainPC
{
	struct LetterboxMarker
	{};

	template<class T>
	concept letterbox = std::derived_from<T, LetterboxMarker>;

	namespace Implement
	{
		template<class T>
		void check_shared_ptr(const std::shared_ptr<T>&);
	}

	template<class T>
	concept callback_shared_ptr = requires(const T& imut, const DataField data)
	{
		typename T::weak_type;
		std::weak_ptr{imut};
		std::weak_ptr{imut}.lock();
		std::weak_ptr{imut}.lock()->callback(data);
	};

	namespace Implement
	{
		inline constexpr struct DummyCallback
		{
			void callback(const DataField&)
			{}
		} dummy_callback{};
	}

	template<class T>
	concept letterbox_maker = std::move_constructible<T> && requires(T mut, const T imut, const DataField data)
	{
		mut(std::make_shared<std::remove_cvref_t<decltype(Implement::dummy_callback)>>());
	};
}