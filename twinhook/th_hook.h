#pragma once

class th_hook
{
protected:
	~th_hook() = default;
public:
	th_hook(){}

	virtual void hook() = 0;
};