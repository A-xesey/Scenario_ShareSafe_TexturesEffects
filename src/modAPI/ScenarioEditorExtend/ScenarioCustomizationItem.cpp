#include "stdafx.h"
#include "Global.h"
#include "ScenarioCustomizationItem.h"

using namespace App;

ScenarioCustomizationItem::ScenarioCustomizationItem()
	: mpButton(nullptr)
	, mpTooltipWinProc(nullptr)
	, mbIsSelected(false)
{
}


ScenarioCustomizationItem::~ScenarioCustomizationItem()
{
}

void ScenarioCustomizationItem::SetCustomizationAndImage(
	const PropertyList* pPropList,
	ResourceKey thumbnailKey,
	IWinProc* pHandler
)
{
	LoadByID(CONTROL_ID_CUSTOMIZATION_ITEM);

	Property::GetKey(pPropList, PROPERTY_ID_CUSTOMIZATION_ITEM_KEY, mCustomizationKey);
	mThumbnailKey = thumbnailKey;
	Property::GetText(pPropList, PROPERTY_ID_CUSTOMIZATION_ITEM_NAME, mName);
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
// For internal use, do not modify.
int ScenarioCustomizationItem::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int ScenarioCustomizationItem::Release()
{
	return DefaultRefCounted::Release();
}
#pragma endregion
