#pragma once

#include "vec2_d.hpp"

namespace CRSLib::LightweightPointCloud::LShapeFit
{
	struct Pose2D final
	{
		Vec2D point;
		double theta;

		static auto zero()
		{
			return Pose2D{Vec2D{}, 0};
		}

		friend auto operator*(const Pose2D& l, const double r) -> Pose2D
		{
			return {l.point * r, l.theta * r};
		}

		friend auto operator-(const Pose2D& l, const Pose2D& r) -> Pose2D
		{
			return {l.point - r.point, l.theta - r.theta};
		}

		friend auto operator+(const Pose2D& l, const Pose2D& r) -> Pose2D
		{
			return {l.point + r.point, l.theta + r.theta};
		}

		auto operator+=(const Pose2D& x) & -> Pose2D&
		{
			point += x.point;
			theta += x.theta;
			return *this;
		}
	};
}