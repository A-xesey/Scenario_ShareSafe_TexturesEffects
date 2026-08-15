#include "stdafx.h"
#include "ScenarioFloraGroundCoverLock.h"

ScenarioFloraGroundCoverLock::ScenarioFloraGroundCoverLock(
	ScenarioEditModeSculptFloraUI* pEditModeSculptFloraUI,
	bool mIsLocked
)
	: mbIsLocked(mIsLocked)
{
	if (pEditModeSculptFloraUI)
	{
		uintptr_t ptr = (uintptr_t)pEditModeSculptFloraUI;
		UILayout* pLayout = (UILayout*)field(ptr, 0x168);
		IWindow* pFloraGroundCoverWin = pLayout->
			FindWindowByID(CONTROL_ID_PALETTE_FLORA_GROUND_COVER);
		if (pFloraGroundCoverWin)
			for (IWindow* pChildWin : pFloraGroundCoverWin->children())
				if (IButton* pChildBtn = ((IButton*)pChildWin->Cast(IButton::TYPE)))
				{
					mWinLockMap[pChildWin] = !pChildWin->IsEnabled();
					if (SporeStdDrawable* pDrawable = (SporeStdDrawable*)(pChildWin->
						GetDrawable()->Cast(SporeStdDrawable::TYPE)))
					{
						SporeStdDrawableImageInfo* pDrawableImageInfo = pDrawable->GetImageInfo(1);
						pDrawableImageInfo->SetIconDrawMode(IconDrawModes::WindowSize);
						Color iconColor = pDrawableImageInfo->GetIconColor();
						Vector3 iconColorHSV;
						Color::RGBToHSV(
							iconColor.r, iconColor.g, iconColor.b,
							iconColorHSV.x, iconColorHSV.y, iconColorHSV.z
						);
						Color::HSVToRGB(
							iconColorHSV.x, iconColorHSV.y, COLOR_VALUE_DISABLED,
							iconColor.r, iconColor.g, iconColor.b
						);
						pDrawableImageInfo->SetIconColor(iconColor);
					}
				}
	}
}


ScenarioFloraGroundCoverLock::~ScenarioFloraGroundCoverLock()
{
}



void ScenarioFloraGroundCoverLock::SetLock(bool bLock)
{
	if (mbIsLocked == bLock)
		return;
	for (pair<IWindow* const, bool>& pairWinLock : mWinLockMap)
	{
		bool bIsEnabled = !bLock && !pairWinLock.second;
		pairWinLock.first->SetEnabled(bIsEnabled);
		if (IButton* pBtn = ((IButton*)pairWinLock.first->Cast(IButton::TYPE)))
			pBtn->SetButtonPressed(false);
	}
	mbIsLocked = bLock;
}

#pragma region Refcount
// For internal use, do not modify.
int ScenarioFloraGroundCoverLock::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int ScenarioFloraGroundCoverLock::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* ScenarioFloraGroundCoverLock::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(ScenarioFloraGroundCoverLock);
	return nullptr;
}
#pragma endregion
