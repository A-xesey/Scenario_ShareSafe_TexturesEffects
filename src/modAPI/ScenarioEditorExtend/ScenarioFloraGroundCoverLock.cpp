#include "stdafx.h"
#include "ScenarioFloraGroundCoverLock.h"

ScenarioFloraGroundCoverLock::ScenarioFloraGroundCoverLock(
	ScenarioEditModeSculptFloraUI* pEditModeSculptFloraUI,
	bool bIsLocked
)
	: mpWin(nullptr)
	, mpFloraFlowersCheckboxWin(nullptr)
	, mbIsLocked(false)
{
	if (pEditModeSculptFloraUI)
	{
		if (UILayout* pLayout = (UILayout*)field(pEditModeSculptFloraUI, 0x168))
		{
			if (IWindow* pFloraGroundCoverWin = pLayout->
				FindWindowByID(CONTROL_ID_PALETTE_FLORA_GROUND_COVER))
			{
				for (IWindow* pChildWin : pFloraGroundCoverWin->children())
				{
					if (!pChildWin->Cast(IButton::TYPE))
						continue;
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
				mpWin = pFloraGroundCoverWin;
			}
			IWindow* pFloraFlowersCheckboxWin = pLayout->
				FindWindowByID(CONTROL_ID_PALETTE_FLORA_FLOWERS_CHECKBOX);
			if (pFloraFlowersCheckboxWin && pFloraFlowersCheckboxWin->Cast(IButton::TYPE))
			{
				mpFloraFlowersCheckboxWin = pFloraFlowersCheckboxWin;
				mWinLockMap[pFloraFlowersCheckboxWin] = true;
			}
		}
	}
	SetLock(bIsLocked);
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
		pairWinLock.first->SetEnabled(!bLock && !pairWinLock.second);
		((IButton*)pairWinLock.first->Cast(IButton::TYPE))->SetButtonPressed(false);
	}
	mbIsLocked = bLock;
}

void ScenarioFloraGroundCoverLock::Update()
{
	if (mpFloraFlowersCheckboxWin)
		mpFloraFlowersCheckboxWin->SetEnabled(
			!mbIsLocked &&
			!mWinLockMap[mpFloraFlowersCheckboxWin]
		);
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
