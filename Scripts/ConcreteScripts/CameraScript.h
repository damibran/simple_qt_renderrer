#pragma once
#include "../Script.h"
#include "ui_RenderrerMainWindow.h"
#include "../utils/camera.h"

class CameraScript final : public Script
{
public:
	CameraScript(Ui::RenderrerMainWindowClass& ui, Camera* cam):ui_(ui),cam_(cam)
	{
		ui.renderLabel->bindLabelToCamScript(&cam_pos_need_update_, &cam_rot_need_update_);
	}

	void updateScript(float dt) override
	{
		if(cam_pos_need_update_)
		{
			cam_pos_need_update_ = false;

		}
	}

	bool cam_pos_need_update_ = false;
	bool cam_rot_need_update_ = false;
	Camera* cam_;
	Ui::RenderrerMainWindowClass& ui_;
};
