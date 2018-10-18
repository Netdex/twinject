#pragma once

#include <utility>
#include <vector>
#include <windows.h>

class th_patch
{
public:
	enum result
	{
		PATCH_SUCCESS = 0,
		PATCH_MEMORY_VIOLATION,
		PATCH_VERIFY_MISMATCH,
		PATCH_INVALID,
		PATCH_REDUNDANT
	};
	virtual result patch();
	virtual result unpatch();
protected:
	struct th_patch_def
	{
		void* addr;					// pointer to base modification address
		std::vector<char> patch;	// patch contents

		bool shouldVerify = false;	// whether to do verification
		std::vector<char> expected;	// expected unpatched contents

		th_patch_def(void* addr, const char *patch, size_t len)
			: addr(addr), patch(patch, patch + len){}
		th_patch_def(void* addr, const char *patch, const char *expected, size_t len)
			: addr(addr), patch(patch, patch + len), expected(expected, expected + len),
			shouldVerify(true) {}
	};

	th_patch(std::vector<th_patch_def> patches) 
		: patches(std::move(patches)), hProcess(GetCurrentProcess()){}
	virtual ~th_patch() = default;

private:
	bool patched = false;
	std::vector<th_patch_def> patches;
	std::vector<th_patch_def> original;
	HANDLE hProcess = NULL;

	result apply_patch(th_patch_def *patch) const;
	result validate_patch(th_patch_def *patch) const;
};
