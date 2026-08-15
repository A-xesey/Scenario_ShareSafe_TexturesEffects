#pragma once

#include "Global.h"

#define ScenarioFloraGroundCoverLockPtr intrusive_ptr<ScenarioFloraGroundCoverLock>

using namespace UTFWin;

class ScenarioFloraGroundCoverLock 
	: public Object
	, public DefaultRefCounted
{
protected:
	map<IWindow*, bool> mWinLockMap;
	bool mbIsLocked;

private:
#pragma region Constants
	static const uint32_t CONTROL_ID_PALETTE_FLORA_GROUND_COVER = 0x7d61ff2;

	static constexpr float COLOR_VALUE_DISABLED = 0.75f;
#pragma endregion

public:
	static const uint32_t TYPE = id(PrivateName("ScenarioFloraGroundCoverLock"));
	
	ScenarioFloraGroundCoverLock(
		ScenarioEditModeSculptFloraUI* pEditModeSculptFloraUI,
		bool mIsLocked = false
	);
	~ScenarioFloraGroundCoverLock();

	void SetLock(bool bLock);

	inline void Lock() { SetLock(true); }
	inline void Unlock() { SetLock(false); }
	inline bool IsLocked() { return mbIsLocked; }

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
};
