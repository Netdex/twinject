#pragma once

class th_patch
{
public:
	th_patch(){}
	virtual ~th_patch() = default;

	virtual void patch() = 0;
	virtual void unpatch() = 0;
};
