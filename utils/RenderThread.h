#pragma once
#include "../MyMain/Scene.h"
#include "../utils/Semaphore.h" 
#include <thread>

class RenderThread
{
public:

	RenderThread(const uint mx, const uint my, uint _thread_count,
	             Scene& scene_,
	             bool& working_): pool_(mx / 2, my / 2, _thread_count), screen_(mx, my), scene_(scene_),
	                              working_(working_)
	{
		s = new semaphore();
		pool_.sleep_duration = 0;
	}

	void start()
	{
		t = std::thread(&RenderThread::work, this);
	}

	thread_pool pool_;
	Screen screen_;

	~RenderThread()
	{
		t.join();
		delete s;
	}

private:
	void work()
	{
		while (working_)
		{
			tp2_ = std::chrono::system_clock::now();
			const std::chrono::duration<float> elapsed_time = tp2_ - tp1_;
			tp1_ = tp2_;
			const float delta_time = elapsed_time.count();
			qDebug()<<1./delta_time;

			pool_.clearBuffers({200, 200, 200});
			//updating all scene
			scene_.updateScene(delta_time);

			scene_.renderScene(pool_);

			s->acquire();

			pool_.swapBuffer();

			pool_.push_task([this](ThreadContext& cntx)
			{
				screen_.sumUpBuffers(pool_.prev_buffer,pool_,s);
			});
		}
	}

	semaphore* s;
	std::chrono::system_clock::time_point tp1_ = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point tp2_ = std::chrono::system_clock::now();
	Scene& scene_;
	bool& working_;
	std::thread t;
};
