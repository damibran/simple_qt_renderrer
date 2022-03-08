#pragma once
#include"../MyMain/Screen.h"
#include "../utils/MVPMat.h"
#include "../utils/LineSegment.h"
#include<array>
#include <map>

class BezierCurveRenderer : public RendererComponent
{
public:
	BezierCurveRenderer(Screen& s,
	                    const int initial_depth) : screen_(s), depth_(initial_depth)
	{
		//subDivide(0, 1, 1);
	}

	void drawShapeVisual(const MVPMat& trans) override
	{
		glm::mat4 fullMat = trans.proj * trans.view * trans.model;

		if (need_update_)
		{
			need_update_ = false;
			subDivide(0, 1, 1);
		}

		for (auto& i : curve_patch_)
		{
			glm::vec4 a = fullMat * glm::vec4(i.second.a, 1.0f);
			glm::vec4 b = fullMat * glm::vec4(i.second.b, 1.0f);

			if (a.w < 0.1f && b.w >= 0.1f)
			{
				clipNearInClipSpace(a, b);
			}

			if (b.w < 0.1f && a.w >= 0.1f)
			{
				clipNearInClipSpace(b, a);
			}

			a.x = static_cast<float>(screen_.XMAX) * ((a.x / a.w + 1) / 2);
			a.y = static_cast<float>(screen_.YMAX) * ((a.y / a.w + 1) / 2);

			b.x = static_cast<float>(screen_.XMAX) * ((b.x / b.w + 1) / 2);
			b.y = static_cast<float>(screen_.YMAX) * ((b.y / b.w + 1) / 2);

			if (a.w > 0 && b.w > 0)
				bresenhamWTest(a, b);
		}
	}

	std::array<glm::vec3, 5>* getControlPointsPtr()
	{
		return &control_points_;
	}

	bool* getNeedUpdatePtr()
	{
		return &need_update_;
	}

private:
	static void clipNearInClipSpace(glm::vec4& a, const glm::vec4& b) // a is outside, b inside
	{
		float d_1 = -(b.z + 0.1f);
		float d_2 = -(a.z + 0.1f);
		// eqval to  float d_2 = glm::dot(glm::vec3(a) - glm::vec3(0, 0, -0.1f), glm::vec3(0, 0, -1));
		float t = d_1 / (d_1 - d_2);
		a = b + t * (a - b);
	}

	void subDivide(float start, float end, int cur_depth)
	{
		if (cur_depth != depth_)
		{
			const float centr = start + (end - start) / 2;
			subDivide(start, centr, cur_depth + 1);
			subDivide(centr, end, cur_depth + 1);
		}
		else
		{
			LineSeg n;
			n.a = decasteljau(start);
			n.b = decasteljau(end);
			curve_patch_[end] = n;
		}
	}

	static glm::vec3 lerp(const glm::vec3& a, const glm::vec3 b, float t)
	{
		return (1 - t) * a + t * b;
	}

	glm::vec3 decasteljau(float t) const
	{
		// compute first tree points along main segments P1-P2, P2-P3 and P3-P4
		glm::vec3 P12 = lerp(control_points_[0], control_points_[1], t);
		glm::vec3 P23 = lerp(control_points_[1], control_points_[2], t);
		glm::vec3 P34 = lerp(control_points_[2], control_points_[3], t);
		glm::vec3 P45 = lerp(control_points_[3], control_points_[4], t);
		// compute two points along segments P1P2-P2P3 and P2P3-P3P4
		glm::vec3 P1223 = lerp(P12, P23, t);
		glm::vec3 P2334 = lerp(P23, P34, t);
		glm::vec3 P3445 = lerp(P34, P45, t);

		glm::vec3 P1223_2334 = lerp(P1223, P2334, t);
		glm::vec3 P2334_3445 = lerp(P2334, P3445, t);

		// finally compute P
		return lerp(P1223_2334, P2334_3445, t); // P 
	}

	void bresenhamWTest(const glm::vec2& v1, const glm::vec2& v2) const
	{
		const float x1 = v1.x;
		const float y1 = v1.y;

		const float x2 = v2.x;
		const float y2 = v2.y;

		const float xdiff = (x2 - x1);
		const float ydiff = (y2 - y1);

		if (xdiff == 0.0f && ydiff == 0.0f)
		{
			if (x1 > 0 && x1 < static_cast<float>(screen_.XMAX) && y1 > 0 && y1 < static_cast<float>(screen_.YMAX))
				screen_.put_point(static_cast<uint>(x1), static_cast<uint>(y1), {0, 0, 0});
			return;
		}

		if (fabs(xdiff) > fabs(ydiff))
		{
			float xmin, xmax;

			// set xmin to the lower x value given
			// and xmax to the higher value
			if (x1 < x2)
			{
				xmin = x1;
				xmax = x2;
			}
			else
			{
				xmin = x2;
				xmax = x1;
			}

			// draw line in terms of y slope
			const float slope = ydiff / xdiff;
			for (float x = xmin; x <= xmax; x += 1.0f)
			{
				float y = y1 + ((x - x1) * slope);
				if (x > 0 && x < static_cast<float>(screen_.XMAX) && y > 0 && y < static_cast<float>(screen_.YMAX))
					screen_.put_point(static_cast<uint>(x), static_cast<uint>(y), {0, 0, 0});
			}
		}
		else
		{
			float ymin, ymax;

			// set ymin to the lower y value given
			// and ymax to the higher value
			if (y1 < y2)
			{
				ymin = y1;
				ymax = y2;
			}
			else
			{
				ymin = y2;
				ymax = y1;
			}

			// draw line in terms of x slope
			float slope = xdiff / ydiff;
			for (float y = ymin; y <= ymax; y += 1.0f)
			{
				float x = x1 + ((y - y1) * slope);
				if (x > 0 && x < static_cast<float>(screen_.XMAX) && y > 0 && y < static_cast<float>(screen_.YMAX))
					screen_.put_point(static_cast<uint>(x), static_cast<uint>(y), {0, 0, 0});
			}
		}
	}

	Screen& screen_;
	int depth_;
	bool need_update_=true;
	std::array<glm::vec3, 5> control_points_;
	std::map<float, LineSeg> curve_patch_;
};
