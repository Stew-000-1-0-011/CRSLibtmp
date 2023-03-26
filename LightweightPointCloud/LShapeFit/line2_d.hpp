#pragma once

#include <optional>

#include "vec2_d.hpp"

namespace CRSLib::LightweightPointCloud::LShapeFit
{
	/**
		 * @brief 最小二乗法で2次元平面上の直線を当てはめるためのクラス。
		 * 
		 */
		class Line2D final
		{
			double x_2 = 0;
			double y_2 = 0;
			double xy = 0;
			double x = 0;
			double y = 0;

			public:
			/**
			 * @brief あるデータ点を加える関数。
			 * 
			 * @param vec
			 */
			void update(const Vec2D& vec) noexcept
			{
				x_2 += vec[0] * vec[0];
				y_2 += vec[1] * vec[1];
				xy += vec[0] * vec[1];
				x += vec[0];
				y += vec[1];
			}

			std::optional<blaze::StaticVector<double, 2>> calc_ab() const noexcept
			{
				auto a = blaze::StaticMatrix<double, 2, 2>
				{
					{x_2, xy},
					{xy, y_2}
				};

				const auto b = blaze::StaticVector<double, 2>{-x, -y};

				try
				{
					blaze::invert(a);
				}
				catch(std::invalid_argument&)
				{
					return std::nullopt;
				}

				return a * b;
			}
		};
}