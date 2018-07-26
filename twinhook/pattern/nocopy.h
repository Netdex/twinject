#pragma once

class nocopy
{
public:
	nocopy(nocopy const&) = delete;
	nocopy& operator=(nocopy const&) = delete;
	nocopy() {}
};