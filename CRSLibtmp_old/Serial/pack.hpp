#pragma once

#include <vector>

#include <CRSLibtmp/std_type.hpp>
#include <CRSLibtmp/utility.hpp>

namespace CRSLib::Serial
{
	struct Decobsed;
	struct Cobsed;

	// decobsedなデータ [a, b, c, d]
	struct Decobsed final
	{
		std::vector<u8> decobsed;

		explicit Decobsed(cvref_same<decltype(decobsed)> auto&& decobsed) noexcept:
			decobsed{std::forward<decltype(decobsed)>(decobsed)}
		{}

		auto cobs() const -> Cobsed;
	};

	// cobsedなデータ [distance_to_next_zero, a, b, c, d, 0]  0終端を忘れずに
	struct Cobsed final
	{
		std::vector<u8> cobsed;

		explicit Cobsed(cvref_same<decltype(cobsed)> auto&& cobsed) noexcept:
			cobsed{std::forward<decltype(cobsed)>(cobsed)}
		{}

		auto decobs() const -> Decobsed;
	};

	auto Decobsed::cobs() const -> Cobsed
	{
		std::vector<u8> cobsed(decobsed.size() + 2);
		
		{
			auto previous_zero = cobsed.begin();
			auto cobsed_it = cobsed.begin() + 1;
			for(const auto e : decobsed)
			{
				if(e != 0)
				{
					*cobsed_it = e;
				}
				else
				{
					*previous_zero = cobsed_it - previous_zero;
					previous_zero = cobsed_it;
				}

				++cobsed_it;
			}
			*cobsed_it = 0;
		}

		return Cobsed{std::move(cobsed)};
	}

	auto Cobsed::decobs() const -> Decobsed
	{
		std::vector<u8> decobsed(cobsed.size() - 2);
		
		{
			size_t next_zero = cobsed[0];
			size_t decobsed_i = 0;
			for(auto& e : decobsed)
			{
				if(decobsed_i == next_zero)
				{
					e = 0;
					next_zero += cobsed[next_zero];
				}
				else
				{
					e = cobsed[decobsed_i + 1];
					--next_zero;
				}
				++decobsed_i;
			}
		}

		return decobsed;
	}

	// inline std::vector<u8> cobs(const std::vector<u8>& frame) noexcept
	// {
	// 	std::vector<u8> ret(frame.size() + 2, 0);
		
	// 	u8 * previous_zero_element = &ret[0];
	// 	size_t count = 1;  // 次の要素が0なら、その0だった要素には1が入る
	// 	for(size_t i = 0; i < frame.size(); ++i)
	// 	{
	// 		if(frame[i] != 0)
	// 		{
	// 			ret[i + 1] = frame[i];
	// 			++count;
	// 		}
	// 		else
	// 		{
	// 			*previous_zero_element = count;
	// 			previous_zero_element = &ret[i + 1];
	// 			count = 1;
	// 		}
	// 	}

	// 	return ret;
	// }

	// inline std::vector<u8> decobs(const std::vector<u8>& cobsed_frame) noexcept
	// {
	// 	std::vector<u8> ret(cobsed_frame.size() - 2);
		
	// 	size_t next_zero = cobsed_frame[0];
	// 	for(size_t i = 0; i < ret.size(); ++i)
	// 	{
	// 		if(next_zero == 1)
	// 		{
	// 			next_zero = cobsed_frame[i + 1];
	// 			ret[i] = 0;
	// 		}
	// 		else
	// 		{
	// 			ret[i] = cobsed_frame[i + 1];
	// 			--next_zero;
	// 		}
	// 	}

	// 	return ret;
	// }
}