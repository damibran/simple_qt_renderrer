#pragma once
#include "../shapes/Mesh.h"
#include "../ThreadPool.h"

class MeshRenderer
{
public:
	MeshRenderer(Shader& shdr, std::unique_ptr<Mesh> m) :
		shader(shdr)
		, mesh(std::move(m))
	{
		for (int i = 0; i < countThreads; ++i)
		{
			shadersQ.push(std::move(shader.prototypeCopy()));
		}
	}
	void drawMesh(Screen& screen, const MVP_mat& trans)
	{
		drawMesh(screen, trans, mesh);
	}
private:

	void drawMesh(Screen& screen, const MVP_mat& trans, std::unique_ptr<Mesh> const& _mesh)
	{
		{
			ThreadPool pool(countThreads);
			std::condition_variable cond;

			for (int i = 0; _mesh->indices.size() != 0 && i <= _mesh->indices.size() - 3; i += 3)
			{
				pool.enqueue([this, &cond, &screen, &trans, &_mesh, i]()
					{
						std::unique_lock<std::mutex> lock(qMut);
				
						while(shadersQ.empty())
							cond.wait(lock);
				
						std::unique_ptr tempShader = std::move(shadersQ.front());
						shadersQ.pop();
				
						lock.unlock();
						screen.process_trngl(tempShader, trans, _mesh->vertices[_mesh->indices[i]], _mesh->vertices[_mesh->indices[i + 1]], _mesh->vertices[_mesh->indices[i + 2]]);
						lock.lock();
						shadersQ.push(std::move(tempShader));
						lock.unlock();
						cond.notify_one();
					}
				);
			}
		}

		for (int i = 0; i < _mesh->childs.size(); ++i)
		{
			drawMesh(screen, trans, _mesh->childs[i]);
		}
	}
	size_t countThreads = 2;
	std::mutex qMut;
	std::queue<std::unique_ptr<Shader>> shadersQ;
	Shader& shader;
	const std::unique_ptr<Mesh> mesh;
};