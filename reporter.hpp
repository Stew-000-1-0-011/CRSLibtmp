#pragma once

#include <concepts>
#include <iostream>
#include <syncstream>


namespace CRSLib
{
	template<class T>
	concept reporter = std::move_constructible<T> &&
	requires(T reporter, const char *const c_str)
	{
		{reporter(c_str)} noexcept;
	};

	struct DefaultErrorReporter final
	{
		void operator()(const auto& ... args) const noexcept
		{
			try
			{
				std::osyncstream sync_err{std::cerr};
				(sync_err << ... << args) << std::endl;
			}
			catch(...)
			{
				try
				{
					std::osyncstream sync_err{std::cerr};
					sync_err << "Fail to report error." << std::endl;
				}
				catch(...){}
			}
		}
	};

	struct DefaultInfoReporter final
	{
		void operator()(const auto& ... args) const noexcept
		{
			try
			{
				std::osyncstream sync_out{std::cout};
				(sync_out << ... << args) << std::endl;
			}
			catch(...)
			{
				try
				{
					std::osyncstream sync_out{std::cerr};
					sync_out << "Fail to report info." << std::endl;
				}
				catch(...){}
			}
		}
	};
}