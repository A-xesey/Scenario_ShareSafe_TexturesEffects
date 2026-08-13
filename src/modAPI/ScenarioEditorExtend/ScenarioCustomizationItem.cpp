#include "stdafx.h"
#include "Global.h"
#include "ScenarioCustomizationItem.h"

ScenarioCustomizationItem::ScenarioCustomizationItem()
	: mpButton(nullptr)
	, mpTooltipWinProc(nullptr)
	, mbIsSelected(false)
{
}


ScenarioCustomizationItem::~ScenarioCustomizationItem()
{
}

using namespace App;

//static std::u16string uintToString(const unsigned int& i) {
//	unsigned int num = i;
//	std::string s;
//	while (num != 0)
//	{
//		int mod = num % 16;
//		if (mod >= 10)
//		{
//			if (mod == 10) s = "A" + s;
//			if (mod == 11) s = "B" + s;
//			if (mod == 12) s = "C" + s;
//			if (mod == 13) s = "D" + s;
//			if (mod == 14) s = "E" + s;
//			if (mod == 15) s = "F" + s;
//		}
//		else s = std::to_string(mod) + s;
//		num /= 16;
//	}
//	while (s.length() != 8) s = "0" + s;
//	return std::u16string(s.begin(), s.end());
//}

void ScenarioCustomizationItem::SetCustomizationAndImage(const PropertyList* pPropList, ResourceKey thumbnailKey, IWinProc* pHandler)
{
	LoadByID(CONTROL_ID_CUSTOMIZATION_ITEM);

	Property::GetKey(pPropList, PROPERTY_ID_CUSTOMIZATION_ITEM_KEY, mCustomizationKey);
	mThumbnailKey = thumbnailKey;
	Property::GetText(pPropList, PROPERTY_ID_CUSTOMIZATION_ITEM_NAME, mName);
	IWindowPtr pButton = FindWindowByID(CONTROL_ID_CUSTOMIZATION_ITEM);
	IWindowPtr pThumbnailWin = pButton->FindWindowByID(CONTROL_ID_CUSTOMIZATION_ITEM_THUMBNAIL);
	if (pThumbnailWin) //ResourceManager.FindRecord(mThumbnailKey)
		Image::SetBackgroundByKey(pThumbnailWin.get(), mThumbnailKey);

	if (pButton)
	{
		mpTooltipWinProc = CreateTooltip(mName.GetText());
		pButton->AddWinProc(pHandler);
		pButton->AddWinProc(mpTooltipWinProc.get());
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


