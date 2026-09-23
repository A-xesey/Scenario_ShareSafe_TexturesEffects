#include "stdafx.h"
#include "Global.h"
#include "ScenarioCustomizationItem.h"

using namespace App;

ScenarioCustomizationItem::ScenarioCustomizationItem()
    : mbIsSelected(false)
    , mbIgnoreGroundCoverLock(false)
    , mpButton(nullptr)
    , mpTooltipWinProc(nullptr)
    , mszName(nullptr)
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
    Property* pPropertyName = nullptr;
    if (pPropList->GetProperty(PROPERTY_ID_CUSTOMIZATION_ITEM_NAME, pPropertyName))
    {
        Property::TextProperty* pString = (Property::TextProperty*)pPropertyName->GetValueText();
        mszName = pString->tableID && pString->instanceID
            ? LocalizedString(pString->tableID, pString->instanceID).GetText()
            : pString->buffer;
    }
    else
        mszName = LocalizedString().GetText();
    Property::GetBool(
        pPropList,
        PROPERTY_ID_CUSTOMIZATION_ITEM_IS_GROUND_COVER,
        mbIgnoreGroundCoverLock
    );
    if (IWindow* pButton = FindWindowByID(CONTROL_ID_CUSTOMIZATION_ITEM))
    {
        if (IWindow* pThumbnail = pButton->FindWindowByID(CONTROL_ID_CUSTOMIZATION_ITEM_THUMBNAIL))
            Image::SetBackgroundByKey(pThumbnail, mThumbnailKey);
        else
            return false;
        mpCursorWin = pButton->FindWindowByID(CONTROL_ID_CUSTOMIZATION_ITEM_CURSOR);
        mpTooltipWinProc = CreateTooltip(mszName);
        pButton->AddWinProc(pHandler);
        pButton->AddWinProc(mpTooltipWinProc);
        mpButton = pButton;
    }

    SetSelection(false);
    return true;
}

void ScenarioCustomizationItem::SetSelection(bool bIsSelected)
{
    if (mpCursorWin)
        mpCursorWin->SetVisible(bIsSelected);
    mbIsSelected = bIsSelected;
}

#pragma region Refcount
int ScenarioCustomizationItem::AddRef()
{
    return DefaultRefCounted::AddRef();
}

int ScenarioCustomizationItem::Release()
{
    return DefaultRefCounted::Release();
}
#pragma endregion
