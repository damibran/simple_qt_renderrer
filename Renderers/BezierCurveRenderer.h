#pragma once
#include "Renderer.h"
#include"../Screen.h"
#include "../utils/MVP_mat.h"
#include<array>
#include <map>

struct line
{
	glm::vec3 a;
	glm::vec3 b;
};

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

			if (checkPoint(a) && checkPoint(b))
				bresenham(a.x, a.y, b.x, b.y);
		}
	}

private:

	bool checkPoint(const glm::vec4& p)
	{
		return (p.x > 0 && p.x < screen.XMAX && p.y>0 && p.y < screen.YMAX);
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
			line n;
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

	void bresenham(int xa, int ya, int xb, int yb)
	{
		int dx = abs(xa - xb), dy = abs(ya - yb);
		int p = 2 * dy - dx;
		int twoDy = 2 * dy, twoDyDx = 2 * (dy - dx);
		int x, y, xEnd;
		/*Determine which points to start and End */
		if (xa > xb) {
			x = xb;
			y = yb;
			xEnd = xa;
		}
		else {
			x = xa; y = ya; xEnd = xb;
		}
		screen.put_point(x, y, { 0,0,0 });
		while (x < xEnd) {
			x++;
			if (p < 0) {
				p = p + twoDy;
			}
			else {
				y++;
				p = p + twoDyDx;
			}
			screen.put_point(x, y, { 0,0,0 });
		}

	}

	Screen& screen;
	int depth;
	std::array<glm::vec3, 4> controlPoints;
	std::map<float, line> curvePatch;
};