#pragma once
#include <Windows.h>



class Input
{
public:
	enum class CameraAction
	{
		NOTHING, UP, DOWN, LEFT, RIGHT, ZOOMOUT, ZOOMIN
	};
	static CameraAction getCameraAction()
	{
		CameraAction res= CameraAction::NOTHING;

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
			res = CameraAction::UP;
		else if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
			res = CameraAction::DOWN;
		else if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			res = CameraAction::LEFT;
		else if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			res = CameraAction::RIGHT;
		else if (GetAsyncKeyState((unsigned short)'Q') & 0x8000)
			res = CameraAction::ZOOMOUT;
		else if (GetAsyncKeyState((unsigned short)'E') & 0x8000)
			res = CameraAction::ZOOMIN;

		return res;
	}

	static bool getExitAction()
	{
		bool res = false;
		if (GetAsyncKeyState((unsigned short)27) & 0x8000)
			res = true;
		return res;
	}
};