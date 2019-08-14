#include "stdafx.h"
#include "th_patch.h"

th_patch::result th_patch::apply_patch(th_patch_def *patch) const
{
	if (!WriteProcessMemory(hProcess, patch->addr,
		&*patch->patch.begin(), patch->patch.size(), NULL)) {
		SPDLOG_ERROR("FATAL: memory violation on patch apply");
		return PATCH_MEMORY_VIOLATION;
	}
	return PATCH_SUCCESS;
}

th_patch::result th_patch::validate_patch(th_patch_def *patch) const
{
	if(patch->shouldVerify)
	{
		if (patch->patch.size() != patch->expected.size()) {
			SPDLOG_ERROR("sub patch vs expected size mismatch");
			return PATCH_INVALID;
		}
		if (memcmp(&*patch->expected.begin(), patch->addr, patch->patch.size()) != 0) {
			SPDLOG_ERROR("sub patch verification does not match");
			return PATCH_VERIFY_MISMATCH;
		}
	}
	return PATCH_SUCCESS;
}

th_patch::result th_patch::patch()
{
	if (patched) {
		SPDLOG_WARN("tried to patch but redundant");
		return PATCH_REDUNDANT;
	}

	original.clear();
	for (auto i : patches)
	{
		SPDLOG_INFO("applying sub patch to 0x%08x of length %d", i.addr, i.patch.size());
		result validationResult = validate_patch(&i);
		if (validationResult != PATCH_SUCCESS) {
			SPDLOG_ERROR("patch validation failed with code %x", validationResult);
			return validationResult;
		}

		original.push_back(th_patch_def(i.addr, (const char*)i.addr, i.patch.size()));
		result patchResult = apply_patch(&i);
		if (patchResult != PATCH_SUCCESS) {
			SPDLOG_ERROR("patch application failed with code %x", patchResult);
			return patchResult;
		}
	}
	patched = true;
	return PATCH_SUCCESS;
}

th_patch::result th_patch::unpatch()
{
	if (!patched) {
		SPDLOG_WARN("tried to unpatch but redundant");
		return PATCH_REDUNDANT;
	}
	for (auto i : original)
	{
		SPDLOG_INFO("applying sub patch to 0x%08x of length %d", i.addr, i.patch.size());
		result patchResult = apply_patch(&i);
		if (patchResult != PATCH_SUCCESS)
			return patchResult;
	}
	patched = false;
	return PATCH_SUCCESS;
}