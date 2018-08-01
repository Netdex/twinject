#pragma once

#include <memory>

class th_patch
{

protected:
	struct th_patch_def
	{
		void* addr;
		std::vector<char> patch;

		th_patch_def(void* addr, const char *patch, size_t len)
			: addr(addr), patch(patch, patch + len){}
	};

	th_patch(std::vector<th_patch_def> patches) 
		: patches(patches), hProcess(GetCurrentProcess()){}
	virtual ~th_patch() = default;

private:
	bool patched = false;
	std::vector<th_patch_def> patches;
	std::vector<th_patch_def> original;
	HANDLE hProcess = NULL;

	void apply_patch(th_patch_def *patch) const
	{
		WriteProcessMemory(hProcess, patch->addr, 
			&*patch->patch.begin(), patch->patch.size(), NULL);
	}

public:
	virtual void patch()
	{
		if (patched)
			return;

		original.clear();
		for (auto i : patches)
		{
			original.push_back(th_patch_def(i.addr, (const char*) i.addr, i.patch.size()));
			apply_patch(&i);
		}
		patched = true;
	}
	virtual void unpatch()
	{
		if (!patched)
			return;
		for (auto i : original)
		{
			apply_patch(&i);
		}
		patched = false;
	}
};
