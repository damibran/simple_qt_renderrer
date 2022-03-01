#pragma once
#include "Renderer.h"
#include"../Screen.h"
#include "../utils/MVP_mat.h"
#include "../utils/LineSegment.h"
#include<array>
#include<algorithm>
#include <map>

class BezierCurveRenderer : public Renderer
{
public:
	BezierCurveRenderer(Screen& s, std::array<glm::vec3, 4> cP, int initial_depth) :screen(s), depth(initial_depth), controlPoints(cP)
	{
		subDevide(0, 1, 1);
	}

	void drawShapeVisual(const MVP_mat& trans) override
	{
		glm::mat4 fullMat = trans.proj * trans.view * trans.model;
		for (auto& i : curvePatch)
		{
			glm::vec4 a = fullMat * glm::vec4(i.second.a, 1.0f);
			glm::vec4 b = fullMat * glm::vec4(i.second.b, 1.0f);

			a.x = (a.x / a.w + 1) / 2 * screen.XMAX;
			a.y = (a.y / a.w + 1) / 2 * screen.YMAX;

			b.x = (b.x / b.w + 1) / 2 * screen.XMAX;
			b.y = (b.y / b.w + 1) / 2 * screen.YMAX;

			bresenham(cropPoint(a), cropPoint(b));
		}
	}

private:

	glm::vec2 cropPoint(const glm::vec4& p)
	{
		return glm::vec2(std::max(std::min(p.x, (float)screen.XMAX - 1), 0.0f), std::max(std::min(p.y, (float)screen.YMAX - 2), 0.0f));
	}

	void subDevide(float start, float end, int cur_depth)
	{
		if (cur_depth != depth) {
			float centr = start + (end - start) / 2;
			subDevide(start, centr, cur_depth + 1);
			subDevide(centr, end, cur_depth + 1);
		}
		else
		{
			line_seg n;
			n.a = decasteljau(start);
			n.b = decasteljau(end);
			curvePatch[end] = n;
		}
	}

	glm::vec3 decasteljau(float t)
	{
		// compute first tree points along main segments P1-P2, P2-P3 and P3-P4
		glm::vec3 P12 = (1 - t) * controlPoints[0] + t * controlPoints[1];
		glm::vec3 P23 = (1 - t) * controlPoints[1] + t * controlPoints[2];
		glm::vec3 P34 = (1 - t) * controlPoints[2] + t * controlPoints[3];
		// compute two points along segments P1P2-P2P3 and P2P3-P3P4
		glm::vec3 P1223 = (1 - t) * P12 + t * P23;
		glm::vec3 P2334 = (1 - t) * P23 + t * P34;

		// finally compute P
		return (1 - t) * P1223 + t * P2334; // P 
	}

	void bresenham(glm::vec2 v1, glm::vec2 v2)
	{
		float x1 = v1.x;
		float y1 = v1.y;

		float x2 = v2.x;
		float y2 = v2.y;

		float xdiff = (x2 - x1);
		float ydiff = (y2 - y1);

		if (xdiff == 0.0f && ydiff == 0.0f) {
			screen.put_point(x1, y1, { 0,0,0 });
			return;
		}

		if (fabs(xdiff) > fabs(ydiff)) {
			float xmin, xmax;

			// set xmin to the lower x value given
			// and xmax to the higher value
			if (x1 < x2) {
				xmin = x1;
				xmax = x2;
			}
			else {
				xmin = x2;
				xmax = x1;
			}

			// draw line in terms of y slope
			float slope = ydiff / xdiff;
			for (float x = xmin; x <= xmax; x += 1.0f) {
				float y = y1 + ((x - x1) * slope);
				screen.put_point(x, y, { 0,0,0 });
			}
		}
		else {
			float ymin, ymax;

			// set ymin to the lower y value given
			// and ymax to the higher value
			if (y1 < y2) {
				ymin = y1;
				ymax = y2;
			}
			else {
				ymin = y2;
				ymax = y1;
			}

			// draw line in terms of x slope
			float slope = xdiff / ydiff;
			for (float y = ymin; y <= ymax; y += 1.0f) {
				float x = x1 + ((y - y1) * slope);
				screen.put_point(x, y, { 0,0,0 });
			}
		}
	}

	Screen& screen;
	int depth;
	std::array<glm::vec3, 4> controlPoints;
	std::map<float, line_seg> curvePatch;
};