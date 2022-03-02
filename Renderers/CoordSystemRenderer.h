#pragma once
#pragma once
#include "Renderer.h"
#include"../Screen.h"
#include "../utils/MVP_mat.h"
#include<array>
#include <map>

class CoordSystemRenderer : public Renderer
{
public:
	CoordSystemRenderer(Screen& s) :screen(s) {}

	void drawShapeVisual(const MVP_mat& trans) override
	{
		glm::mat4 fullMat = trans.proj * trans.view * trans.model;

		drawAixs(fullMat, { 1,0,0 }, { -1,0,0 });
		drawAixs(fullMat, { 0,1,0 }, { 0,-1,0 });
		drawAixs(fullMat, { 0,0,1 }, { 0,0,-1 });
	}

private:

	void drawAixs(const glm::mat4& fullMat, const glm::vec3& _a, const glm::vec3& _b)
	{
		glm::vec4 a = fullMat * glm::vec4(_a, 1.0f);
		glm::vec4 b = fullMat * glm::vec4(_b, 1.0f);

		if (a.w < 0.1f && b.w >= 0.1f)
		{
			clipNearInViewSpace(a, b);
		}

		if (b.w < 0.1f && a.w >= 0.1f)
		{
			clipNearInViewSpace(b, a);
		}

		a.x = (a.x / a.w + 1) / 2 * screen.XMAX;
		a.y = (a.y / a.w + 1) / 2 * screen.YMAX;

		b.x = (b.x / b.w + 1) / 2 * screen.XMAX;
		b.y = (b.y / b.w + 1) / 2 * screen.YMAX;

		if (a.w > 0 && b.w > 0)
			bresenhamWTest(a, b);
	}

	void clipNearInHomoSpace(glm::vec4& a, const glm::vec4& b) // a is outside, b inside
	{
		float t = (b.w + b.z) / (b.w + b.z - (a.w + a.z));
		a = b + t * (a - b);
	}

	void clipNearInViewSpace(glm::vec4& a, const glm::vec4& b) // a is outside, b inside
	{
		float d_1 = -(b.z + 0.1f);
		float d_2 = -(a.z + 0.1f);// eqval to  float d_2 = glm::dot(glm::vec3(a) - glm::vec3(0, 0, -0.1f), glm::vec3(0, 0, -1));
		float t = d_1 / (d_1 - d_2);
		a = b + t * (a - b);
	}

	glm::vec2 cropPoint(const glm::vec4& p)
	{
		return glm::vec2(std::max(std::min(p.x, (float)screen.XMAX - 1), 0.0f), std::max(std::min(p.y, (float)screen.YMAX - 1), 1.0f));
	}

	void bresenhamWTest(const glm::vec2& v1, const glm::vec2& v2)
	{
		float x1 = v1.x;
		float y1 = v1.y;

		float x2 = v2.x;
		float y2 = v2.y;

		float xdiff = (x2 - x1);
		float ydiff = (y2 - y1);

		if (xdiff == 0.0f && ydiff == 0.0f) {
			if (x1 > 0 && x1 < screen.XMAX && y1>0 && y1 < screen.YMAX)
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
				if (x > 0 && x < screen.XMAX && y>0 && y < screen.YMAX)
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
				if (x > 0 && x < screen.XMAX && y>0 && y < screen.YMAX)
					screen.put_point(x, y, { 0,0,0 });
			}
		}
	}

	Screen& screen;
};