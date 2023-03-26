#pragma once

#include <optional>

#include <blaze/Math.h>

#include <CRSLibtmp/std_type.hpp>

#include "pose2_d.hpp"
#include "line2_d.hpp"
#include "vec2_d.hpp"

namespace CRSLib::LightweightPointCloud::LShapeFit
{
	inline auto calc_l_shape(const std::vector<Vec2D>& data_points, const Pose2D& l_shape_from_odometry, const double threshold) -> std::optional<Pose2D>
	{
		const auto axis_x = Vec2D{blaze::cos(l_shape_from_odometry.theta), blaze::sin(l_shape_from_odometry.theta)};
		const auto axis_y = Vec2D{-blaze::sin(l_shape_from_odometry.theta), blaze::cos(l_shape_from_odometry.theta)};

		Line2D fitted_x{}, fitted_y{};
		Vec2D sum_x{};
		u64 x_n = 0, y_n = 0;

		for(const Vec2D& data_point : data_points)
		{
			const auto point = data_point - l_shape_from_odometry.point;
			const auto unit_point = blaze::trans(blaze::normalize(point));

			if(const auto dot_x = blaze::acos(unit_point * axis_x); dot_x <= blaze::min(-threshold, -0.5) || blaze::max(threshold, 0.5) <= dot_x)
			{
				fitted_x.update(point);
				sum_x += point;
				++x_n;
			}
			else if(const auto dot_y = blaze::acos(unit_point * axis_y); dot_y <= blaze::min(-threshold, -0.5) || blaze::max(threshold, 0.5) <= dot_y)
			{
				fitted_y.update(point);
				++y_n;
			}
		}

		if(const auto x_normal = fitted_x.calc_ab(); x_normal) if(const auto y_normal = fitted_y.calc_ab(); y_normal)
		{
			if(const auto cross = (*x_normal)[0] * (*y_normal)[1] - (*x_normal)[1] * (*y_normal)[0]; cross != 0)
			{
				Pose2D ret;
				ret.point = Vec2D{(*x_normal)[1] - (*y_normal)[1], (*y_normal)[0] - (*x_normal)[0]} / cross;
				
				// よっぽどのことがなければx軸の反時計周りpi/2にy軸がある。よって、ここではy軸を考える必要がない。
				Vec2D mettya_x_ppoi_axis = blaze::sign(blaze::trans(*y_normal) * sum_x) * *y_normal;
				if(y_n < x_n)
				{
					const auto ppoi_y_nomral = Vec2D{-(*x_normal)[1], (*x_normal)[0]};
					mettya_x_ppoi_axis = blaze::sign(blaze::trans(ppoi_y_nomral) * sum_x) * ppoi_y_nomral;
				}

				ret.theta = blaze::atan2(mettya_x_ppoi_axis[1], mettya_x_ppoi_axis[0]);

				return {ret};
			}
		}

		return std::nullopt;
	}
}