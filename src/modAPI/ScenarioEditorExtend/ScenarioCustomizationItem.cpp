#include "stdafx.h"
#include "Global.h"
#include "ScenarioCustomizationItem.h"

using namespace App;

ScenarioCustomizationItem::ScenarioCustomizationItem()
	: mbIsSelected(false)
	, mbIgnoreGroundCoverLock(false)
	, mpButton(nullptr)
	, mpTooltipWinProc(nullptr)
{
}


ScenarioCustomizationItem::~ScenarioCustomizationItem()
{
}

bool ScenarioCustomizationItem::SetCustomizationAndImage(
	const PropertyList* pPropList,
	ResourceKey thumbnailKey,
	IWinProc* pHandler
)
{
	LoadByID(CONTROL_ID_CUSTOMIZATION_ITEM);

	if (!Property::GetKey(pPropList, PROPERTY_ID_CUSTOMIZATION_ITEM_KEY, mCustomizationKey))
		return false;
	mThumbnailKey = thumbnailKey;
	if (!Property::GetText(pPropList, PROPERTY_ID_CUSTOMIZATION_ITEM_NAME, mName))
		mName.SetText(0x0, 0x0);
	Property::GetBool(
		pPropList,
		PROPERTY_ID_CUSTOMIZATION_ITEM_IS_GROUND_COVER,
		mbIgnoreGroundCoverLock
	);
	IWindow* pButton = FindWindowByID(CONTROL_ID_CUSTOMIZATION_ITEM);
	IWindow* pThumbnailWin = pButton->FindWindowByID(CONTROL_ID_CUSTOMIZATION_ITEM_THUMBNAIL);
	if (pThumbnailWin) //ResourceManager.FindRecord(mThumbnailKey)
		Image::SetBackgroundByKey(pThumbnailWin, mThumbnailKey);

	if (pButton)
	{
		mpTooltipWinProc = CreateTooltip(mName.GetText());
		pButton->AddWinProc(pHandler);
		pButton->AddWinProc(mpTooltipWinProc);
		mpButton = pButton;
	}

	SetSelection(false);
}

void ScenarioCustomizationItem::SetSelection(bool bIsSelected)
{
	IWindowPtr pCursorWin = FindWindowByID(CONTROL_ID_CUSTOMIZATION_ITEM_CURSOR);
	pCursorWin->SetVisible(bIsSelected);
	mbIsSelected = bIsSelected;
}

#pragma region Refcount
int ScenarioCustomizationItem::AddRef()
{
	return DefaultRefCounted::AddRef();
}

int ScenarioCustomizationItem::Release()
{
	int refCount = DefaultRefCounted::Release();
	if (!refCount)
		g_nScenarioCustomizationCategoryIndex = -1;
	return refCount;
}
#pragma endregion
