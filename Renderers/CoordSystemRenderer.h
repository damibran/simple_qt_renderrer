#pragma once
#pragma once
#include "RendererComponent.h"
#include"../Screen.h"
#include "../utils/MVPMat.h"

class CoordSystemRenderer : public RendererComponent
{
public:
	CoordSystemRenderer(Screen& s) : screen(s)
	{
	}

	void drawShapeVisual(const MVPMat& trans) override
	{
		glm::mat4 fullMat = trans.proj * trans.view * trans.model;

		drawAxis(fullMat, {1, 0, 0}, {-1, 0, 0});
		drawAxis(fullMat, {0, 1, 0}, {0, -1, 0});
		drawAxis(fullMat, {0, 0, 1}, {0, 0, -1});
	}

private:
	void drawAxis(const glm::mat4& full_mat, const glm::vec3& a_in, const glm::vec3& b_in) const
	{
		glm::vec4 a_res = full_mat * glm::vec4(a_in, 1.0f);
		glm::vec4 b_res = full_mat * glm::vec4(b_in, 1.0f);

		if (a_res.w < 0.1f && b_res.w >= 0.1f)
		{
			clipNearInClipSpace(a_res, b_res);
		}

		if (b_res.w < 0.1f && a_res.w >= 0.1f)
		{
			clipNearInClipSpace(b_res, a_res);
		}

		a_res.x = (a_res.x / a_res.w + 1) / 2 * static_cast<float>(screen.XMAX);
		a_res.y = (a_res.y / a_res.w + 1) / 2 * static_cast<float>(screen.YMAX);

		b_res.x = (b_res.x / b_res.w + 1) / 2 * static_cast<float>(screen.XMAX);
		b_res.y = (b_res.y / b_res.w + 1) / 2 * static_cast<float>(screen.YMAX);

		if (a_res.w > 0 && b_res.w > 0)
			bresenhamWTest(a_res, b_res);
	}

	static void clipNearInHomoSpace(glm::vec4& a, const glm::vec4& b) // a is outside, b inside
	{
		float t = (b.w + b.z) / (b.w + b.z - (a.w + a.z));
		a = b + t * (a - b);
	}

	static void clipNearInClipSpace(glm::vec4& a, const glm::vec4& b) // a is outside, b inside
	{
		const float d_1 = -(b.z + 0.1f);
		const float d_2 = -(a.z + 0.1f);
		// eqval to  float d_2 = glm::dot(glm::vec3(a) - glm::vec3(0, 0, -0.1f), glm::vec3(0, 0, -1));
		float t = d_1 / (d_1 - d_2);
		a = b + t * (a - b);
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
			if (x1 > 0 && x1 < static_cast<float>(screen.XMAX) && y1 > 0 && y1 < static_cast<float>(screen.YMAX))
				screen.put_point(static_cast<uint>(x1), static_cast<uint>(y1), {0, 0, 0});
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
				if (x > 0 && x < static_cast<float>(screen.XMAX) && y > 0 && y < static_cast<float>(screen.YMAX))
					screen.put_point(static_cast<uint>(x), static_cast<uint>(y), {0, 0, 0});
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
				if (x > 0 && x < static_cast<float>(screen.XMAX) && y > 0 && y < static_cast<float>(screen.YMAX))
					screen.put_point(static_cast<uint>(x), static_cast<uint>(y), {0, 0, 0});
			}
		}
	}

	Screen& screen;
};
