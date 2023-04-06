#pragma once

#include <optional>
#include <numbers>

#include <blaze/Math.h>

#include <CRSLibtmp/std_type.hpp>
#include <CRSLibtmp/Math/vec2_d.hpp>

#include <CRSLibtmp/Math/pose2_d.hpp>
#include "line2_d.hpp"

namespace CRSLib::LightweightPointCloud::LShapeFit
{
	/**
	 * @brief 与えられた点群データとL字の位置姿勢の推定値から、L字の位置姿勢(交点の位置と横棒の向き)を計算する。
	 * 
	 * @param data_points 点群データ。ある時刻におけるものとして補正済みである必要がある
	 * @param l_shape_from_odometry L字の位置姿勢の推定値。きっと1ステップ前の位置姿勢にオドメトリ情報なんかを足して作るかと思う。
	 * @param threshold しきい値。データ点のうち、その点とL字交点とL字の各棒それぞれの成す角[rad]がこれ以下であるもののみを利用する。 デフォルトでπ/8。
	 * @return std::optional<Math::Pose2D> 
	 */
	inline auto calc_l_shape(const std::vector<Math::Vec2D>& data_points, const Math::Pose2D& l_shape_from_odometry, const double threshold = std::numbers::pi / 8) -> std::optional<Math::Pose2D>
	{
		const double fixed_threshold = std::min(threshold, std::numbers::pi / 4);
		const auto axis_x = Math::Vec2D{blaze::cos(l_shape_from_odometry.theta), blaze::sin(l_shape_from_odometry.theta)};
		const auto axis_y = Math::Vec2D{-blaze::sin(l_shape_from_odometry.theta), blaze::cos(l_shape_from_odometry.theta)};

		Line2D fitted_x{}, fitted_y{};
		Math::Vec2D sum_x{};
		u64 x_n = 0, y_n = 0;

		for(const Math::Vec2D& data_point : data_points)
		{
			const auto point = data_point - l_shape_from_odometry.point;
			const auto unit_point = blaze::trans(blaze::normalize(point));

			if(const auto dot_x = blaze::acos(unit_point * axis_x); blaze::abs(dot_x) <= fixed_threshold)
			{
				fitted_x.update(point);
				sum_x += point;
				++x_n;
			}
			else if(const auto dot_y = blaze::acos(unit_point * axis_y); blaze::abs(dot_y) <= fixed_threshold)
			{
				fitted_y.update(point);
				++y_n;
			}
		}

		if(const auto x_normal = fitted_x.calc_ab(); x_normal) if(const auto y_normal = fitted_y.calc_ab(); y_normal)
		{
			if(const auto cross = (*x_normal)[0] * (*y_normal)[1] - (*x_normal)[1] * (*y_normal)[0]; cross != 0)
			{
				Math::Pose2D ret;
				ret.point = Math::Vec2D{(*x_normal)[1] - (*y_normal)[1], (*y_normal)[0] - (*x_normal)[0]} / cross;
				
				// よっぽどのことがなければx軸の反時計周りpi/2にy軸がある。よって、ここではy軸を考える必要がない。
				Math::Vec2D mettya_x_ppoi_axis = blaze::sign(blaze::trans(*y_normal) * sum_x) * *y_normal;
				if(y_n < x_n)
				{
					const auto ppoi_y_nomral = Math::Vec2D{-(*x_normal)[1], (*x_normal)[0]};
					mettya_x_ppoi_axis = blaze::sign(blaze::trans(ppoi_y_nomral) * sum_x) * ppoi_y_nomral;
				}

				ret.theta = blaze::atan2(mettya_x_ppoi_axis[1], mettya_x_ppoi_axis[0]);

				return {ret};
			}
		}

		return std::nullopt;
	}
}