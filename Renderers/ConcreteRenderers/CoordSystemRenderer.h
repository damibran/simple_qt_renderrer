#pragma once
#pragma once
#include"../MyMain/Screen.h"
#include "../utils/MVPMat.h"
#include "../utils/RenderThread.h"

class CoordSystemRenderer final : public RendererComponent
{
public:
	explicit CoordSystemRenderer()=default;

	void drawShapeVisual(thread_pool& pool,const MVPMat& trans) override
	{
		const glm::mat4 full_mat = trans.proj * trans.view * trans.model;

		pool.push_task([this,full_mat](ThreadContext& cntx)
				{
					drawAxis(cntx,full_mat, {1, 0, 0}, {-1, 0, 0},{180,0,0});
					drawAxis(cntx,full_mat, {0, 1, 0}, {0, -1, 0},{0,180,0});
					drawAxis(cntx,full_mat, {0, 0, 1}, {0, 0, -1},{0,0,180});
				});
	}

private:
	void drawAxis(ThreadContext& cntx,const glm::mat4& full_mat, const glm::vec3& a_in, const glm::vec3& b_in, const glm::vec3& color) const
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

		a_res.x = (a_res.x / a_res.w + 1) / 2 * static_cast<float>(cntx.w_);
		a_res.y = (a_res.y / a_res.w + 1) / 2 * static_cast<float>(cntx.h_);
		a_res.z = a_res.w;

		b_res.x = (b_res.x / b_res.w + 1) / 2 * static_cast<float>(cntx.w_);
		b_res.y = (b_res.y / b_res.w + 1) / 2 * static_cast<float>(cntx.h_);
		b_res.z = b_res.w;

		if (a_res.w > 0 && b_res.w > 0)
			bresenhamWTest(cntx,a_res, b_res, color);
	}

	static void clipNearInHomoSpace(glm::vec4& a, const glm::vec4& b) // a is outside, b inside
	{
		const float t = (b.w + b.z) / (b.w + b.z - (a.w + a.z));
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

	void bresenhamWTest(ThreadContext&cntx, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& color) const
	{
		const glm::vec3 diff = (v2 - v1);

		if (diff.x == 0.0f && diff.y == 0.0f)
		{
			if (v1.x > 0 && v1.x < static_cast<float>(cntx.w_) && v1.y > 0 && v1.y < static_cast<float>(cntx.h_))
			{
				cntx.setColorBufferAt(v1.x,v1.y, color);
				cntx.setZBufferAt(v1.x,v1.y,FLT_MIN);
			}
			return;
		}

		if (fabs(diff.x) > fabs(diff.y))
		{
			float xmin, xmax;

			// set xmin to the lower x value given
			// and xmax to the higher value
			if (v1.x < v2.x)
			{
				xmin = v1.x;
				xmax = v2.x;
			}
			else
			{
				xmin = v2.x;
				xmax = v1.x;
			}

			// draw line in terms of y slope
			const float slope = diff.y / diff.x;
			for (float x = xmin; x <= xmax; x += 1.0f)
			{
				float y = v1.y + ((x - v1.x) * slope);
				float t=x/xmax;
				//float z=x
				if (x > 0 && x < static_cast<float>(cntx.w_) && y > 0 && y < static_cast<float>(cntx.h_))
				{
					cntx.setColorBufferAt(x,y, color);
					cntx.setZBufferAt(x,y,FLT_MIN);
				}
			}
		}
		else
		{
			float ymin, ymax;

			// set ymin to the lower y value given
			// and ymax to the higher value
			if (v1.y < v2.y)
			{
				ymin = v1.y;
				ymax = v2.y;
			}
			else
			{
				ymin = v2.y;
				ymax = v1.y;
			}

			// draw line in terms of x slope
			float slope = diff.x / diff.y;
			for (float y = ymin; y <= ymax; y += 1.0f)
			{
				float x = v1.x + ((y - v1.y) * slope);
				if (x > 0 && x < static_cast<float>(cntx.w_) && y > 0 && y < static_cast<float>(cntx.h_))
				{
					cntx.setColorBufferAt(x,y, color);
					cntx.setZBufferAt(x,y,FLT_MIN);
				}
			}
		}
	}
};
