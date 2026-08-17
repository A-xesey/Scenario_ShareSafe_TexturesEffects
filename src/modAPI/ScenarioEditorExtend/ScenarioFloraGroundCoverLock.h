#pragma once

#include "Global.h"

#define ScenarioFloraGroundCoverLockPtr intrusive_ptr<ScenarioFloraGroundCoverLock>

using namespace UTFWin;

class ScenarioFloraGroundCoverLock 
	: public Object
	, public DefaultRefCounted
{
protected:
	IWindow* mpWin;
	IWindow* mpFloraFlowersCheckboxWin;
	map<IWindow*, bool> mWinLockMap;
	bool mbIsLocked;

private:
#pragma region Constants
	static constexpr float COLOR_VALUE_DISABLED = 0.7f;

	static const uint32_t CONTROL_ID_PALETTE_FLORA_FLOWERS_CHECKBOX = 0x16b1858d;
#pragma endregion

public:
	static const uint32_t TYPE = id(PrivateName("ScenarioFloraGroundCoverLock"));
	
	ScenarioFloraGroundCoverLock(
		ScenarioEditModeSculptFloraUI* pEditModeSculptFloraUI,
		bool mIsLocked = false
	);
	~ScenarioFloraGroundCoverLock();

	void SetLock(bool bLock);
	void Update();

	inline IWindow* GetInitializedWindow() { return mpWin; }
	inline void Lock() { SetLock(true); }
	inline void Unlock() { SetLock(false); }
	inline bool IsLocked() { return mbIsLocked; }

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
};
