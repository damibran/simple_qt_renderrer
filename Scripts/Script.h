#pragma once

class Script
{
public:
	virtual ~Script() = default;
	virtual void updateScript(float dt) = 0;
};