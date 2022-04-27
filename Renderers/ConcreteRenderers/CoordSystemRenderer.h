#pragma once
#pragma once
#include"../MyMain/Screen.h"
#include "../utils/MVPMat.h"
#include "../utils/RenderThread.h"

class CoordSystemRenderer final : public RendererComponent
{
public:
	explicit CoordSystemRenderer() = default;

	void drawShapeVisual(thread_pool& pool, const MVPMat& trans) override
	{
		const glm::mat4 full_mat = trans.proj * trans.view * trans.model;

		pool.push_task([this,full_mat](ThreadContext& cntx)
		{
			drawAxis(cntx, full_mat, {1, 0, 0}, {-1, 0, 0}, {180, 0, 0});
			drawAxis(cntx, full_mat, {0, 1, 0}, {0, -1, 0}, {0, 180, 0});
			drawAxis(cntx, full_mat, {0, 0, 1}, {0, 0, -1}, {0, 0, 180});
		});
	}

private:
	void drawAxis(ThreadContext& cntx, const glm::mat4& full_mat, const glm::vec3& a_in, const glm::vec3& b_in,
	              const glm::vec3& color) const
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
			bresenham3D(a_res, b_res, cntx, color);
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

	void bresenham3D(const glm::vec3& v1, const glm::vec3& v2, ThreadContext& cntx, const glm::vec3& color)const
	{
		int i, err_1, err_2;

		glm::vec3 point = v1;
		const int dx = v2.x - v1.x;
		const int dy = v2.y - v1.y;
		const int dz = v2.z - v1.z;
		const int x_inc = (dx < 0) ? -1 : 1;
		const int l = abs(dx);
		const int y_inc = (dy < 0) ? -1 : 1;
		const int m = abs(dy);
		const int z_inc = (dz < 0) ? -1 : 1;
		const int n = abs(dz);
		const int dx2 = l << 1;
		const int dy2 = m << 1;
		const int dz2 = n << 1;

		if ((l >= m) && (l >= n))
		{
			err_1 = dy2 - l;
			err_2 = dz2 - l;
			for (i = 0; i < l; i++)
			{
				//output->getTileAt(point[0], point[1], point[2])->setSymbol(symbol);
				if (point.x > 0 && point.x < static_cast<float>(cntx.w_) && point.y > 0 && point.y < static_cast<float>(cntx.h_))
				{
					cntx.setColorBufferAt(point.x, point.y, color);
					cntx.setZBufferAt(point.x, point.y, point.z);
				}
				if (err_1 > 0)
				{
					point[1] += y_inc;
					err_1 -= dx2;
				}
				if (err_2 > 0)
				{
					point[2] += z_inc;
					err_2 -= dx2;
				}
				err_1 += dy2;
				err_2 += dz2;
				point[0] += x_inc;
			}
		}
		else if ((m >= l) && (m >= n))
		{
			err_1 = dx2 - m;
			err_2 = dz2 - m;
			for (i = 0; i < m; i++)
			{
				//output->getTileAt(point[0], point[1], point[2])->setSymbol(symbol);
				if (point.x > 0 && point.x < static_cast<float>(cntx.w_) && point.y > 0 && point.y < static_cast<float>(cntx.h_))
				{
					cntx.setColorBufferAt(point.x, point.y, color);
					cntx.setZBufferAt(point.x, point.y, point.z);
				}
				if (err_1 > 0)
				{
					point[0] += x_inc;
					err_1 -= dy2;
				}
				if (err_2 > 0)
				{
					point[2] += z_inc;
					err_2 -= dy2;
				}
				err_1 += dx2;
				err_2 += dz2;
				point[1] += y_inc;
			}
		}
		else
		{
			err_1 = dy2 - n;
			err_2 = dx2 - n;
			for (i = 0; i < n; i++)
			{
				//output->getTileAt(point[0], point[1], point[2])->setSymbol(symbol);
				if (point.x > 0 && point.x < static_cast<float>(cntx.w_) && point.y > 0 && point.y < static_cast<float>(cntx.h_))
				{
					cntx.setColorBufferAt(point.x, point.y, color);
					cntx.setZBufferAt(point.x, point.y, point.z);
				}
				if (err_1 > 0)
				{
					point[1] += y_inc;
					err_1 -= dz2;
				}
				if (err_2 > 0)
				{
					point[0] += x_inc;
					err_2 -= dz2;
				}
				err_1 += dy2;
				err_2 += dx2;
				point[2] += z_inc;
			}
		}
		//output->getTileAt(point[0], point[1], point[2])->setSymbol(symbol);
		if (point.x > 0 && point.x < static_cast<float>(cntx.w_) && point.y > 0 && point.y < static_cast<float>(cntx.h_))
		{
			cntx.setColorBufferAt(point.x, point.y, color);
			cntx.setZBufferAt(point.x, point.y, point.z);
		}
	}
};
