#pragma once
#include"../MyMain/Screen.h"
#include "../utils/MVPMat.h"
#include "../utils/LineSegment.h"
#include<array>
#include <map>

struct cmpPairBezierTs
{
	bool operator()(const std::pair<float, float>& a, const std::pair<float, float>& b) const
	{
		if (a.second - a.first == b.second - b.first)
		{
			return a.first < b.first;
		}
		return a.second - a.first > b.second - b.first;
	}
};

class BezierCurveRenderer : public RendererComponent
{
public:
	BezierCurveRenderer(Screen& s,
	                    const int initial_depth) : screen_(s), depth_(initial_depth)
	{
		curve_patch_[{0, 1}] = {{control_points_[0], control_points_[4]}, 0};
	}

	void render(std::map<std::pair<float, float>, std::pair<LineSeg, int>>::iterator cur, const glm::mat4& fullMat,
	            float cur_level)
	{
		float start = cur->first.first;
		float end = cur->first.second;

		const float threshold = 10;
		const float min_level = 5;

		glm::vec4 a = fullMat * glm::vec4(cur->second.first.a, 1.);
		glm::vec4 b = fullMat * glm::vec4(cur->second.first.b, 1.);

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

		if (glm::length(a - b) > threshold && (onScreen(a) || onScreen(b)) ||
			cur_level < min_level)
		{
			const float centr = start + (end - start) / 2;
			auto left = curve_patch_.find({start, centr});
			if (left == curve_patch_.end())
			{
				left = curve_patch_.insert({
					{start, centr}, {{decasteljau(start), decasteljau(centr)}, last_update}
				}).first;
			}
			if (left->second.second < last_update)
			{
				left->second.first.a = decasteljau(start);
				left->second.first.b = decasteljau(centr);
				left->second.second = last_update;
			}
			auto right = curve_patch_.find({centr, end});
			if (right == curve_patch_.end())
			{
				right = curve_patch_.insert({
					{centr, end}, {{decasteljau(centr), decasteljau(end)}, last_update}
				}).first;
			}
			if (right->second.second < last_update)
			{
				right->second.first.a = decasteljau(centr);
				right->second.first.b = decasteljau(end);
				right->second.second = last_update;
			}
			render(left, fullMat, ++cur_level);
			render(right, fullMat, ++cur_level);
		}
		else
		{
			if (a.w > 0 && b.w > 0)
				bresenhamWTest(a, b);
		}
	}

	void drawShapeVisual(const MVPMat& trans) override
	{
		glm::mat4 fullMat = trans.proj * trans.view * trans.model;

		if (!need_update_)
		{
			render(curve_patch_.begin(), fullMat, 1);
		}
		else
		{
			++last_update;
			need_update_ = false;
			curve_patch_[{0, 1}] = {{control_points_[0], control_points_[4]}, last_update};
			render(curve_patch_.begin(), fullMat, 1);
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
	bool intersectScreenRectangle(LineSeg seg)
	{
		return doIntersect({0, 0}, {screen_.XMAX, 0}, seg.a, seg.b) ||
			doIntersect({0, 0}, {0, screen_.YMAX}, seg.a, seg.b) ||
			doIntersect({0, screen_.YMAX}, {screen_.XMAX, screen_.YMAX}, seg.a, seg.b) ||
			doIntersect({screen_.XMAX, 0}, {screen_.XMAX, screen_.YMAX}, seg.a, seg.b);
	}

	bool onSegment(glm::vec2 p, glm::vec2 q, glm::vec2 r)
	{
		if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
			q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
			return true;

		return false;
	}

	int orientation(glm::vec2 p, glm::vec2 q, glm::vec2 r)
	{
		// See https://www.geeksforgeeks.org/orientation-3-ordered-points/
		// for details of below formula.
		int val = (q.y - p.y) * (r.x - q.x) -
			(q.x - p.x) * (r.y - q.y);

		if (val == 0) return 0; // collinear

		return (val > 0) ? 1 : 2; // clock or counterclock wise
	}

	bool doIntersect(glm::vec2 p1, glm::vec2 q1, glm::vec2 p2, glm::vec2 q2)
	{
		// Find the four orientations needed for general and
		// special cases
		int o1 = orientation(p1, q1, p2);
		int o2 = orientation(p1, q1, q2);
		int o3 = orientation(p2, q2, p1);
		int o4 = orientation(p2, q2, q1);

		// General case
		if (o1 != o2 && o3 != o4)
			return true;

		// Special Cases
		// p1, q1 and p2 are collinear and p2 lies on segment p1q1
		if (o1 == 0 && onSegment(p1, p2, q1)) return true;

		// p1, q1 and q2 are collinear and q2 lies on segment p1q1
		if (o2 == 0 && onSegment(p1, q2, q1)) return true;

		// p2, q2 and p1 are collinear and p1 lies on segment p2q2
		if (o3 == 0 && onSegment(p2, p1, q2)) return true;

		// p2, q2 and q1 are collinear and q1 lies on segment p2q2
		if (o4 == 0 && onSegment(p2, q1, q2)) return true;

		return false; // Doesn't fall in any of the above cases
	}

	bool xInBounds(glm::vec2 a)
	{
		return a.x > 0 && a.x < screen_.XMAX;
	}

	bool yInBounds(glm::vec2 a)
	{
		return a.y > 0 && a.y < screen_.YMAX;
	}

	bool onScreen(glm::vec2 a)
	{
		return xInBounds(a) && yInBounds(a);
	}

	static void clipNearInClipSpace(glm::vec4& a, const glm::vec4& b) // a is outside, b inside
	{
		float d_1 = -(b.z + 0.1f);
		float d_2 = -(a.z + 0.1f);
		// eqval to  float d_2 = glm::dot(glm::vec3(a) - glm::vec3(0, 0, -0.1f), glm::vec3(0, 0, -1));
		float t = d_1 / (d_1 - d_2);
		a = b + t * (a - b);
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
	int last_update = 0;
	bool need_update_ = true;
	std::array<glm::vec3, 5> control_points_;
	std::map<std::pair<float, float>, std::pair<LineSeg, int>, cmpPairBezierTs> curve_patch_;
};
