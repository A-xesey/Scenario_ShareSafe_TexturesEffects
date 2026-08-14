#include "stdafx.h"
#include "ScenarioCustomization.h"
#include <EASTL\sort.h>
#include <Spore\UI\ScrollFrameVertical.h>

using namespace App;
using namespace Audio;
using namespace Simulator;
using namespace Terrain;
using namespace UI;

ScenarioCustomization::ScenarioCustomization(
	IWindowPtr pPaletteCategoryWin,
	IWindowPtr pScrollFrameVerticalWin,
	IWindowPtr pContentClientWin
)
	: mpScenarioTerraformMode(ScenarioMode.GetTerraformMode())
	, mpPaletteCategoryWin(pPaletteCategoryWin)
	, mpScrollFrameVerticalWin(pScrollFrameVerticalWin)
	, mpContentClientWin(pContentClientWin)
	, mpPanelWin(pPaletteCategoryWin->FindWindowByID(CONTROL_ID_CUSTOMIZATION_PANEL))
	, mpSearchboxTextEdit(
		(ITextEdit*)mpPanelWin->FindWindowByID(CONTROL_ID_CUSTOMIZATION_PANEL_SEARCHBOX)->
		Cast(ITextEdit::TYPE)
	)
	, mpPaletteTextureBlockWin(
		mpPaletteCategoryWin->FindWindowByID(CONTROL_ID_PALETTE_BLOCK_TEXTURE)
	)
	, mpPaletteTextureWin(mpPaletteTextureBlockWin->FindWindowByID(CONTROL_ID_PALETTE_BTN_TEXTURE))
	, mpPaletteTextureNameWin(
		mpPaletteTextureBlockWin->FindWindowByID(CONTROL_ID_PALETTE_NAME_TEXTURE)
	)
	//, mpPaletteEffectWin(mpPaletteWinq->FindWindowByID(CONTROL_ID_PALETTE_BTN_EFFECT))
	, mLastLookup(kCustomizationItemsGroupNone, 0)
	, mPropertyTexture(PROPERTY_DEFAULT_TEXTURE)
	, mPropertyEffect(PROPERTY_DEFAULT_EFFECT)
	, mPanelTextureY(
		mpPaletteTextureBlockWin
			? mpPaletteTextureBlockWin->GetRealArea().top
			: 0.0f
	)
	, mpSelectedItem(nullptr)
	, mOpenedPanelItemsGroup(kCustomizationItemsGroupNone)
{
	if (mpPaletteTextureWin)
	{
		CenterUIItem(mpPaletteCategoryWin->FindWindowByID(CONTROL_ID_PALETTE_PROPERTIES_TEXTURE));
		InitItems(
			mpContentClientWin.get(),
			CustomizationItemsLookup(kCustomizationItemsGroupTextures, PROPERTY_DEFAULT_TEXTURE)
		);
		UpdatePaletteTexture();
	}

	//if (mpPaletteEffectWin)
	//...
	// TODO: add the same thing for effects
}

ScenarioCustomization::~ScenarioCustomization()
{
}

void ScenarioCustomization::InitItems(IWindow* pWindow, CustomizationItemsLookup itemsLookup)
{
	if (mLastLookup != itemsLookup && pWindow)
	{
		Math::Rectangle winArea = pWindow->GetRealArea();
		float winWidth = winArea.right - winArea.left;
		float winHeight = winArea.bottom - winArea.top;
		mColumns = (int)((winWidth + ITEM_MARGIN) / ITEM_WIDTH);
		if (mColumns < 1)
			mColumns = 1;

		ClearItems();
		mpSelectedItem = nullptr;
		ResourceKey customizationKeyCurrent = GetCurrentCustomizationKey(itemsLookup.mPropertyId);
		vector<uint32_t> definitionIds;
		PropManager.GetPropertyListIDs(itemsLookup.mItemsGroup, definitionIds);
		if (!definitionIds.empty())
		{
			for (uint32_t definitionId : definitionIds)
			{
				PropertyListPtr pPropList;
				if (!PropManager.GetPropertyList(
					definitionId,
					itemsLookup.mItemsGroup,
					pPropList)
				)
					continue;

#pragma region Blacklist
				vector<uint32_t> itemPropertyBlacklist;
				Property::GetArrayUInt32(
					pPropList.get(),
					PROPERTY_ID_CUSTOMIZATION_ITEM_BLACKLIST,
					itemPropertyBlacklist
				);
				if (
					find(
						itemPropertyBlacklist.begin(),
						itemPropertyBlacklist.end(),
						itemsLookup.mPropertyId
					) != itemPropertyBlacklist.end()
				)
					continue;
#pragma endregion

#pragma region Add Item
				ScenarioCustomizationItemPtr pItem = new ScenarioCustomizationItem();
				pItem->SetCustomizationAndImage(
					pPropList.get(),
					ResourceKey(definitionId, TypeIDs::png, itemsLookup.mItemsGroup),
					this
				);
				string16 itemName = pItem->GetName()->GetText();
				itemName.make_lower();
				if (itemName.find(itemsLookup.mSearchString) == string16::npos)
					continue;
				ResourceKey customizationKey = *pItem->GetCustomization();
				if (customizationKey == customizationKeyCurrent)
				{
					pItem->SetSelection(true);
					mpSelectedItem = pItem;
				}
				mItems.push_back(pItem);
#pragma endregion

			}

#pragma region Sort
			stable_sort(
				mItems.begin(),
				mItems.end(),
				[](
					const ScenarioCustomizationItemPtr& a,
					const ScenarioCustomizationItemPtr& b
				)
				{ return *a->GetCustomization() < *b->GetCustomization(); }
			);
			mItems.erase(
				unique(
					mItems.rbegin(),
					mItems.rend(),
					[](
						const ScenarioCustomizationItemPtr& a,
						const ScenarioCustomizationItemPtr& b
					)
					{ return *a->GetCustomization() == *b->GetCustomization(); }
				),
				mItems.rend()
			);

			quick_sort(
				mItems.begin(),
				mItems.end(),
				[](
					const ScenarioCustomizationItemPtr& a,
					const ScenarioCustomizationItemPtr& b
				)
				{
					const char16_t* pTextA = a->GetName()->GetText();
					const char16_t* pTextB = b->GetName()->GetText();
					return string16::comparei(
						pTextA, pTextA + CharStrlen(pTextA),
						pTextB, pTextB + CharStrlen(pTextB)
					) < 0;
				}
			);
#pragma endregion

#pragma region Register Windows for Items

			size_t index = 0;
			for (ScenarioCustomizationItemPtr pItem : mItems)
			{
				pItem->SetParentWindow(pWindow);
				int row = index / mColumns;
				int col = index % mColumns;
				float positionX = GRID_START_X + (col * ITEM_WIDTH);
				float positionY = GRID_START_Y + (row * ITEM_HEIGHT);

				if (IWindow* pItemWin = pItem->FindWindowByID(CONTROL_ID_CUSTOMIZATION_ITEM))
				{
					Math::Rectangle itemArea = pItemWin->GetArea();

					float width = itemArea.right - itemArea.left;
					float height = itemArea.bottom - itemArea.top;
					itemArea.left = positionX;
					itemArea.top = positionY;
					itemArea.right = positionX + width;
					itemArea.bottom = positionY + height;

					pItemWin->SetArea(itemArea);

					mWinItemMap[pItemWin] = pItem;
				}
				index++;
			}
		}
#pragma endregion

		ScrollFrameVertical::Update(mpScrollFrameVerticalWin.get());
		mLastLookup = itemsLookup;
	}
}

void ScenarioCustomization::ClearItems()
{
	for (pair<IWindow* const, ScenarioCustomizationItemPtr>& pairWinItem : mWinItemMap)
		pairWinItem.first->RemoveWindow(pairWinItem.first);
	mWinItemMap.clear();
	mItems.clear();
}

void ScenarioCustomization::UpdatePaletteTexture()
{
	if (IWindow* pPaletteTextureThumbnailWin = mpPaletteTextureWin->
		FindWindowByID(CONTROL_ID_PALETTE_BTN_TEXTURE_THUMBNAIL))
	{
		ResourceKey customizationKeyCurrent = mpSelectedItem
			? *mpSelectedItem->GetThumbnail()
			: GetCurrentCustomizationKey(mPropertyTexture);
		if (!customizationKeyCurrent.typeID)
			customizationKeyCurrent.typeID = TypeIDs::rw4;
		Image::SetBackgroundByKey(pPaletteTextureThumbnailWin, customizationKeyCurrent);
	}
	if (mpPaletteTextureNameWin)
		mpPaletteTextureNameWin->SetCaption(mpSelectedItem
			? mpSelectedItem->GetName()->GetText()
			: u"-"
		);
}

void ScenarioCustomization::CenterUIItem(IWindowPtr pChildWin)
{
	if (pChildWin)
	{
		Math::Rectangle areaParentWin = pChildWin->GetParent()->GetRealArea();
		Math::Rectangle areaChildWin = pChildWin->GetRealArea();
		
		Vector2 sizeParentWin(
			areaParentWin.GetWidth(),
			areaParentWin.GetHeight()
		);
		Vector2 sizeChildWin(
			areaChildWin.GetWidth(),
			areaChildWin.GetHeight()
		);
		
		areaChildWin.left = (sizeParentWin.x - sizeChildWin.x) / 2.0f;
		areaChildWin.top = (sizeParentWin.y - sizeChildWin.y) / 2.0f;
		areaChildWin.right = areaChildWin.left + sizeChildWin.x;
		areaChildWin.bottom = areaChildWin.top + sizeChildWin.y;
		
		pChildWin->SetLayoutArea(areaChildWin);
	}
}

#pragma region Show & Hide Panel
void ScenarioCustomization::ShowCustomizationPanel(
	CustomizationItemsLookup itemsLookup,
	float positionY
)
{
	ClearSearchbar();
	InitItems(
		mpContentClientWin.get(),
		itemsLookup
	);

	Math::Rectangle areaPanelWin = mpPanelWin->GetArea();
	float heightPanelWin = areaPanelWin.GetHeight();
	areaPanelWin.top = positionY;
	areaPanelWin.bottom = areaPanelWin.top + heightPanelWin;
	mpPanelWin->SetArea(areaPanelWin);

	mpPanelWin->SetVisible(true);
	mpContentClientWin->SetVisible(true);
	mOpenedPanelItemsGroup = itemsLookup.mItemsGroup;
	PlayAudio(SOUND_ID_CUSTOMIZATION_PANEL_OPEN);
}

void ScenarioCustomization::HideCustomizationPanel(bool bSilent)
{
	if (mpPanelWin->IsVisible() && !bSilent)
		PlayAudio(SOUND_ID_CUSTOMIZATION_PANEL_CLOSE);
	mpPanelWin->SetVisible(false);
	mpContentClientWin->SetVisible(false);
	mOpenedPanelItemsGroup = kCustomizationItemsGroupNone;
}
#pragma endregion

#pragma region Refcount
// For internal use, do not modify.
int ScenarioCustomization::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int ScenarioCustomization::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* ScenarioCustomization::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(ScenarioCustomization);
	return nullptr;
}
#pragma endregion

// This method returns a combinations of values in UTFWin::EventFlags.
// The combination determines what types of events (messages) this window procedure receives.
// By default, it receives mouse/keyboard input and advanced messages.
int ScenarioCustomization::GetEventFlags() const
{
	return kEventFlagBasicInput | kEventFlagAdvanced;
}

// The method that receives the message. The first thing you should do is probably
// checking what kind of message was sent...
bool ScenarioCustomization::HandleUIMessage(IWindow* window, const Message& message)
{
	uint32_t controlId;
	switch (message.eventType)
	{
	case kMsgButtonClick:
	{
		controlId = message.source->GetControlID();
		switch (controlId)
		{
		case CONTROL_ID_PALETTE_BTN_TEXTURE: //TODO: rewrite for multiple types
		{
			if (mOpenedPanelItemsGroup == kCustomizationItemsGroupTextures)
				HideCustomizationPanel();
			else
			{
				ShowCustomizationPanel(
					CustomizationItemsLookup(kCustomizationItemsGroupTextures, mPropertyTexture),
					mPanelTextureY
				);
			}
			return true;
		}
		case CONTROL_ID_CUSTOMIZATION_ITEM:
		{
			if (!mpScenarioTerraformMode)
				return true;
			IWindow* pItemWin = message.source;
			ScenarioCustomizationItemPtr pItem = mWinItemMap[pItemWin];
			if (pItem->IsSelected())
				return true;
			mpScenarioTerraformMode->StartHistoryEntry();
			ResourceKey customizationKey = *pItem->GetCustomization();
			PropertyListPtr pTerrainScript = mpScenarioTerraformMode->mpPropList;

			pTerrainScript->SetProperty(
				mLastLookup.mPropertyId,
				&Property().SetValueKey(customizationKey)
			);

			//update current textures
			//cTerrainStateMgr_UpdateFromDefinition
			CALL(
				Address(ModAPI::ChooseAddress(0xf902d0, 0xfbc100)),
				void,
				Args(cTerrainStateMgr*, PropertyList*),
				Args(mpScenarioTerraformMode->mpTerrainStateMgr, pTerrainScript.get())
			);
			//TODO: find a function that actually updates every texture, not just abovedetail2

			SelectItem(pItem);
			UpdatePaletteTexture();
			mpScenarioTerraformMode->CommitHistoryEntry();
			PlayAudio(SOUND_ID_EDITOR_CLICK_GENERAL);
			return true;
		}
		case CONTROL_ID_CUSTOMIZATION_PANEL_SEARCHBOX_BTN_CLEAR:
		{
			ClearSearchbar();
			return true;
		}
		case CONTROL_ID_CUSTOMIZATION_PANEL_CLOSE:
		{
			HideCustomizationPanel();
			return true;
		}
		default:
			return false;
		}
	}
	case kMsgButtonSelect:
	{
		controlId = message.source->GetControlID();
		HideCustomizationPanel(controlId != mPropertyTexture);
		IWindow* pParentWin = message.source->GetParent();
		if (
			pParentWin &&
			pParentWin->GetControlID() == CONTROL_ID_PALETTE_PROPERTIES_TEXTURE &&
			controlId != mPropertyTexture
		)
		{
			mPropertyTexture = (CustomizationPropertyTexture)controlId;
			InitItems(
				mpContentClientWin.get(),
				CustomizationItemsLookup(kCustomizationItemsGroupTextures, mPropertyTexture)
			);
			UpdatePaletteTexture();
			PlayAudio(SOUND_ID_EDITOR_CLICK);
		}
		return true;
	}
	case kMsgTextChanged:
	{
		controlId = message.source->GetControlID();
		if (controlId == CONTROL_ID_CUSTOMIZATION_PANEL_SEARCHBOX)
		{
			if (!mpSearchboxTextEdit)
				return true;
			string16 searchString = mpSearchboxTextEdit->GetText();
			searchString.make_lower();
			if (IWindow* pSearchboxClearWin = message.source->GetParent()->
				FindWindowByID(CONTROL_ID_CUSTOMIZATION_PANEL_SEARCHBOX_BTN_CLEAR))
				pSearchboxClearWin->SetVisible(!searchString.empty());
			CustomizationItemsLookup itemsLookup = mLastLookup;
			itemsLookup.mSearchString = searchString;
			InitItems(
				mpContentClientWin.get(),
				itemsLookup
			);
			return true;
		}
		return false;
	}
	default:
		return false;
	}
}