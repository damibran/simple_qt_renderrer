#pragma once
#include "../MyMain/Screen.h"
#include "../utils/Mesh.h"
#include "../RendererComponent.h"
#include "../Shaders/Shader.h"

class ShaderMeshRenderer : public RendererComponent
{
public:
	~ShaderMeshRenderer() override = default;

	ShaderMeshRenderer(Screen& s, ShaderID shdr, const std::unique_ptr<Mesh>& m) : screen_(s),
		shader_(shdr), mesh_(m), pool_(s.pool_)
	{
	}

	void drawShapeVisual(const MVPMat& trans) override
	{
		drawMesh(screen_, trans, mesh_);
	}

protected:
	void drawMesh(Screen& screen, const MVPMat& trans, std::unique_ptr<Mesh> const& mesh)
	{
		if (!mesh->indices.empty())
		{
			uint count_of_triangles_per_thread = std::ceil(mesh->indices.size() / 3. / screen.pool_.get_thread_count());;
			for (uint t = 0, cur_indx = 0; t < screen.pool_.get_thread_count(); t++, cur_indx +=
			     count_of_triangles_per_thread)
			{
				pool_.push_task([this,cur_indx,count_of_triangles_per_thread,&mesh,&trans](ThreadContext& cntx)
				{
					for (int i = cur_indx; i < cur_indx + count_of_triangles_per_thread && i * 3 < mesh->indices.size(); ++i)
						process_trngl(cntx, shader_, trans, mesh->vertices[mesh->indices[i * 3]],
						              mesh->vertices[mesh->indices[i * 3 + 1]],
						              mesh->vertices[mesh->indices[i * 3 + 2]]);
				});
			}

			//for (size_t i = 0; i <= mesh->indices.size() - 3; i += 3)
			//{
			//}
		}


		for (auto const& i : mesh->childs)
		{
			drawMesh(screen, trans, i);
		}

		screen_.pool_.wait_for_tasks();
	}

	void process_trngl(ThreadContext& cntx, ShaderID shdr, const MVPMat& trans, const Vertex& v0, const Vertex& v1,
	                   const Vertex& v2)
	{
		auto& shader = cntx.shaders_[shdr];
		uint XMAX = cntx.w_;
		uint YMAX = cntx.h_;

		TriangleClipPos abc = shader->computeVertexShader(trans, v0, v1, v2);

		glm::vec3 a;
		glm::vec3 b;
		glm::vec3 c;

		//calculating raster positions
		a.x = static_cast<float>(XMAX) * ((abc.a.x / abc.a.w + 1) / 2);
		a.y = static_cast<float>(YMAX) * ((abc.a.y / abc.a.w + 1) / 2);
		a.z = abc.a.w;

		b.x = static_cast<float>(XMAX) * ((abc.b.x / abc.b.w + 1) / 2);
		b.y = static_cast<float>(YMAX) * ((abc.b.y / abc.b.w + 1) / 2);
		b.z = abc.b.w;

		c.x = static_cast<float>(XMAX) * ((abc.c.x / abc.c.w + 1) / 2);
		c.y = static_cast<float>(YMAX) * ((abc.c.y / abc.c.w + 1) / 2);
		c.z = abc.c.w;

		if (abc.a.z <= abc.a.w && abc.a.z >= -abc.a.w &&
			abc.b.z <= abc.b.w && abc.b.z >= -abc.b.w &&
			abc.c.z <= abc.c.w && abc.c.z >= -abc.c.w) //kinda Clipping
		{
			const float xmin = min3(a.x, b.x, c.x);
			const float ymin = min3(a.y, b.y, c.y);
			const float xmax = max3(a.x, b.x, c.x);
			const float ymax = max3(a.y, b.y, c.y);

			if (xmin > XMAX - 1 || xmax < 0 || ymin > YMAX || ymax < 0)
				return;

			// be careful xmin/xmax/ymin/ymax can be negative. Don't cast to uint32_t
			uint x0 = std::max(0, static_cast<int>(std::floor(xmin)));
			uint x1 = std::min(static_cast<int>(XMAX - 1), static_cast<int>(std::floor(xmax)));
			const uint y0 = std::max(1, static_cast<int>(std::floor(ymin)));
			const uint y1 = std::min(static_cast<int>(YMAX - 1), static_cast<int>(std::floor(ymax)));

			float area = edgeFunction(a, b, c);
			for (uint y = y0; y <= y1; ++y)
			{
				for (uint x = x0; x <= x1; ++x)
				{
					glm::vec2 pixel = {x + 0.5, y + 0.5};

					float w0 = edgeFunction(b, c, pixel);
					float w1 = edgeFunction(c, a, pixel);
					float w2 = edgeFunction(a, b, pixel);

					if (w0 >= 0 && w1 >= 0 && w2 >= 0 || !shader->supportsBackFaceCulling() && w0 <= 0 && w1 <= 0 && w2
						<= 0)
					{
						w0 /= area;
						w1 /= area;
						w2 /= area;

						const float corr = w0 / a.z + w1 / b.z + w2 / c.z;

						w0 /= a.z;
						w1 /= b.z;
						w2 /= c.z;

						w0 /= corr;
						w1 /= corr;
						w2 /= corr;

						const float z = w0 * a.z + w1 * b.z + w2 * c.z;

						if (z < cntx.z_buffer_[y * XMAX + x])
						{
							glm::vec3 color = shader->computeFragmentShader(pixel, w0, w1, w2);

							if (color.x >= 0 && color.y >= 0 && color.z >= 0) // to discard return -1
							{
								cntx.z_buffer_[y * XMAX + x] = z;
								cntx.color_buffer[y * XMAX + x] = color;
							}
						}
					}
				}
			}
		}
	}

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

	Screen& screen_;
	ShaderID shader_;
	const std::unique_ptr<Mesh>& mesh_;
	thread_pool& pool_;
};
