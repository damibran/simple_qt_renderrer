#pragma once
#include "../MyMain/Screen.h"
#include "../utils/Mesh.h"
#include "../RendererComponent.h"
#include "../Shaders/Shader.h"

class ShaderMeshRenderer : public RendererComponent
{
public:

	~ShaderMeshRenderer() override = default;

	ShaderMeshRenderer(Screen& s, std::unique_ptr<Shader> shdr, const std::unique_ptr<Mesh>& m) : screen_(s),
		shader_(std::move(shdr)), mesh_(m)//, pool_(s.pool_)
	{}

	void drawShapeVisual(const MVPMat& trans) override
	{
		drawMesh(screen_, trans, mesh_);
	}

protected:
	void drawMesh(Screen& screen, const MVPMat& trans, std::unique_ptr<Mesh> const& mesh)
	{
		for (size_t i = 0; !mesh->indices.empty() && i <= mesh->indices.size() - 3; i += 3)
		{
			process_trngl(shader_, trans, mesh->vertices[mesh->indices[i]], mesh->vertices[mesh->indices[i + 1]],
			              mesh->vertices[mesh->indices[i + 2]]);
		}

		for (auto const& i : mesh->childs)
		{
			drawMesh(screen, trans, i);
		}
	}

	void process_trngl(std::unique_ptr<Shader>& shader, const MVPMat& trans, const Vertex& v0, const Vertex& v1,
	                   const Vertex& v2)
	{
		TriangleClipPos abc = shader->computeVertexShader(trans, v0, v1, v2);

		glm::vec3 a;
		glm::vec3 b;
		glm::vec3 c;

		//calculating raster positions
		a.x = static_cast<float>(screen_.XMAX) * ((abc.a.x / abc.a.w + 1) / 2);
		a.y = static_cast<float>(screen_.YMAX) * ((abc.a.y / abc.a.w + 1) / 2);
		a.z = abc.a.w;

		b.x = static_cast<float>(screen_.XMAX) * ((abc.b.x / abc.b.w + 1) / 2);
		b.y = static_cast<float>(screen_.YMAX) * ((abc.b.y / abc.b.w + 1) / 2);
		b.z = abc.b.w;

		c.x = static_cast<float>(screen_.XMAX) * ((abc.c.x / abc.c.w + 1) / 2);
		c.y = static_cast<float>(screen_.YMAX) * ((abc.c.y / abc.c.w + 1) / 2);
		c.z = abc.c.w;

		if (abc.a.z <= abc.a.w && abc.a.z >= -abc.a.w &&
			abc.b.z <= abc.b.w && abc.b.z >= -abc.b.w &&
			abc.c.z <= abc.c.w && abc.c.z >= -abc.c.w) //kinda Clipping
			put_triangle(shader, a, b, c);
	}


	void put_triangle(std::unique_ptr<Shader>& shader, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
	{
		const float xmin = min3(v0.x, v1.x, v2.x);
		const float ymin = min3(v0.y, v1.y, v2.y);
		const float xmax = max3(v0.x, v1.x, v2.x);
		const float ymax = max3(v0.y, v1.y, v2.y);

		if (xmin > screen_.XMAX - 1 || xmax < 0 || ymin > screen_.YMAX || ymax < 0)
			return;

		// be careful xmin/xmax/ymin/ymax can be negative. Don't cast to uint32_t
		uint x0 = std::max(0, static_cast<int>(std::floor(xmin)));
		uint x1 = std::min(static_cast<int>(screen_.XMAX - 1), static_cast<int>(std::floor(xmax)));
		const uint y0 = std::max(1, static_cast<int>(std::floor(ymin)));
		const uint y1 = std::min(static_cast<int>(screen_.YMAX - 1), static_cast<int>(std::floor(ymax)));

		float area = edgeFunction(v0, v1, v2);
		for (uint y = y0; y <= y1; ++y)
			{
				for (uint x = x0; x <= x1; ++x)
				{
					glm::vec2 pixel = {x + 0.5, y + 0.5};

					float w0 = edgeFunction(v1, v2, pixel);
					float w1 = edgeFunction(v2, v0, pixel);
					float w2 = edgeFunction(v0, v1, pixel);

					if (w0 >= 0 && w1 >= 0 && w2 >= 0 || !shader->supportsBackFaceCulling() && w0 <= 0 && w1 <= 0 && w2
						<= 0)
					{
						w0 /= area;
						w1 /= area;
						w2 /= area;

						const float corr = w0 / v0.z + w1 / v1.z + w2 / v2.z;

						w0 /= v0.z;
						w1 /= v1.z;
						w2 /= v2.z;

						w0 /= corr;
						w1 /= corr;
						w2 /= corr;

						const float z = w0 * v0.z + w1 * v1.z + w2 * v2.z;

						if (z < screen_.getZBufferAt(y * screen_.XMAX + x))
						{
							glm::vec3 color = shader->computeFragmentShader(pixel, w0, w1, w2);

							if (color.x >= 0 && color.y >= 0 && color.z >= 0)// to discard return -1
							{
								screen_.setZBufferAt(y * screen_.XMAX + x, z);
								screen_.put_point(x, y, color);
							}
						}
					}
				}
			}
		//auto loop = [this, v0, v1, v2, area, x0, x1, &shader](const uint a, const uint b)
		//{
		//	
		//};
		//pool_.parallelize_loop(y0, y1, loop, (y1 - y0) / pool_.get_thread_count());
	}

	Screen& screen_;
	std::unique_ptr<Shader> shader_;
	const std::unique_ptr<Mesh>& mesh_;
	//thread_pool& pool_;

	float min3(const float& a, const float& b, const float& c) const
	{
		return std::min(a, std::min(b, c));
	}

	float max3(const float& a, const float& b, const float& c) const
	{
		return std::max(a, std::max(b, c));
	}

	static float edgeFunction(const glm::vec3& a, const glm::vec3& b, const glm::vec2& c)
	{
		return -((c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x));
	}
};
