#include "stdafx.h"
#include "ScenarioCustomization.h"
#include <EASTL\sort.h>
#include <Spore\UI\ScrollFrameVertical.h>

using namespace Simulator;
using namespace Terrain;
using namespace App;
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
	, mpPaletteTextureWin(mpPaletteCategoryWin->FindWindowByID(CONTROL_ID_PALETTE_BTN_TEXTURE))
	, mpPaletteTextureNameWin(
		mpPaletteCategoryWin->FindWindowByID(CONTROL_ID_PALETTE_NAME_TEXTURE)
	)
	//, mpPaletteEffectWin(mpPaletteWinq->FindWindowByID(CONTROL_ID_PALETTE_BTN_EFFECT))
	, mItemsGroup(kCustomizationItemsGroupNone)
	, mPropertyTexture(PROPERTY_DEFAULT_TEXTURE)
	, mPropertyEffect(PROPERTY_DEFAULT_EFFECT)
	, mpSelectedItem(nullptr)
	, mOpenedPanelItemsGroup(kCustomizationItemsGroupNone)
{
	if (mpPaletteTextureWin)
	{
		Math::Rectangle areaPaletteTextureWin = mpPaletteTextureWin->GetRealArea();
		mPanelTextureY = areaPaletteTextureWin.top;

		InitItems(
			mpContentClientWin.get(),
			kCustomizationItemsGroupTextures,
			PROPERTY_DEFAULT_TEXTURE
		);
		UpdatePaletteTexture();
	}
	// TODO: add the same thing for effects
}

ScenarioCustomization::~ScenarioCustomization()
{
}

void ScenarioCustomization::InitItems(
	IWindow* pWindow,
	CustomizationItemsGroup itemsGroup,
	uint32_t propertyId,
	string16 searchString
)
{
	if ((mItemsGroup != itemsGroup || mSearchString != searchString) && pWindow)
	{
		Math::Rectangle winArea = pWindow->GetRealArea();
		float winWidth = winArea.right - winArea.left;
		float winHeight = winArea.bottom - winArea.top;
		mColumns = (int)((winWidth + ITEM_MARGIN) / ITEM_WIDTH);
		if (mColumns < 1)
			mColumns = 1;

		ClearItems();
		ResourceKey customizationKeyCurrent = GetCurrentCustomizationKey(propertyId);
		vector<uint32_t> definitionIds;
		PropManager.GetPropertyListIDs((uint32_t)itemsGroup, definitionIds);
		if (!definitionIds.empty())
		{
			for (uint32_t definitionId : definitionIds)
			{
				PropertyListPtr pPropList;
				if (!PropManager.GetPropertyList(
					definitionId,
					(uint32_t)itemsGroup,
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
						propertyId
					) != itemPropertyBlacklist.end()
					)
					continue;
#pragma endregion

#pragma region Add Item
				ScenarioCustomizationItemPtr pItem = new ScenarioCustomizationItem();
				pItem->SetCustomizationAndImage(
					pPropList.get(),
					ResourceKey(definitionId, TypeIDs::png, (uint32_t)itemsGroup),
					this
				);
				string16 itemName = pItem->GetName()->GetText();
				itemName.make_lower();
				if (itemName.find(searchString) == string16::npos)
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

				if (IWindowPtr itemWin = pItem->FindWindowByID(CONTROL_ID_CUSTOMIZATION_ITEM))
				{
					Math::Rectangle itemArea = itemWin->GetArea();

					float width = itemArea.right - itemArea.left;
					float height = itemArea.bottom - itemArea.top;
					itemArea.left = positionX;
					itemArea.top = positionY;
					itemArea.right = positionX + width;
					itemArea.bottom = positionY + height;

					itemWin->SetArea(itemArea);

					mWinItemMap[itemWin.get()] = pItem;
				}
				index++;
			}
		}
#pragma endregion

		ScrollFrameVertical::Update(mpScrollFrameVerticalWin.get());
		mItemsGroup = itemsGroup;
		mSearchString = searchString;
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
	if (IWindowPtr pPaletteTextureThumbnailWin = mpPaletteTextureWin->
		FindWindowByID(CONTROL_ID_PALETTE_BTN_TEXTURE_THUMBNAIL))
	{
		ResourceKey customizationKeyCurrent = mpSelectedItem
			? *mpSelectedItem->GetThumbnail()
			: GetCurrentCustomizationKey(mPropertyTexture);
		if (!customizationKeyCurrent.typeID)
			customizationKeyCurrent.typeID = TypeIDs::rw4;
		Image::SetBackgroundByKey(pPaletteTextureThumbnailWin.get(), customizationKeyCurrent);
	}
	if (mpPaletteTextureNameWin)
		mpPaletteTextureNameWin->SetCaption(mpSelectedItem
			? mpSelectedItem->GetName()->GetText()
			: u"-"
		);
}

#pragma region Show & Hide Panel
void ScenarioCustomization::ShowCustomizationPanel(
	CustomizationItemsGroup itemsGroup,
	uint32_t propertyId,
	float positionY
)
{
	ClearSearchbar();
	InitItems(
		mpContentClientWin.get(),
		itemsGroup,
		propertyId
	);

	Math::Rectangle areaPanelWin = mpPanelWin->GetArea();
	float heightPanelWin = areaPanelWin.bottom - areaPanelWin.top;
	areaPanelWin.top = positionY;
	areaPanelWin.bottom = areaPanelWin.top + heightPanelWin;
	mpPanelWin->SetArea(areaPanelWin);

	mpPanelWin->SetVisible(true);
	mpContentClientWin->SetVisible(true);
	mOpenedPanelItemsGroup = itemsGroup;
}

void ScenarioCustomization::HideCustomizationPanel()
{
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
	switch (message.eventType)
	{
	case kMsgButtonClick:
	{
		switch (message.source->GetControlID())
		{
		case CONTROL_ID_PALETTE_BTN_TEXTURE: //TODO: rewrite for multiple types
		{
			if (mOpenedPanelItemsGroup == kCustomizationItemsGroupTextures)
				HideCustomizationPanel();
			else
			{
				ShowCustomizationPanel(
					kCustomizationItemsGroupTextures,
					// TODO: PLACEHOLDER
					kCustomizationPropertyTextureDetail,
					mPanelTextureY
				);
			}
			return true;
		}
		case CONTROL_ID_CUSTOMIZATION_ITEM:
		{
			if (mpScenarioTerraformMode)
			{
				IWindow* pItemWin = message.source;
				ScenarioCustomizationItemPtr pItem = mWinItemMap[pItemWin];
				if (pItem->IsSelected())
					return true;
				mpScenarioTerraformMode->StartHistoryEntry();
				ResourceKey key = *pItem->GetCustomization();
				PropertyListPtr pTerrainScript = mpScenarioTerraformMode->mpPropList;
#pragma region TESTS
				vector<uint32_t> testPropertyIds = {
					PROPERTY_ID_TERRAIN_CLIFF,
					PROPERTY_ID_TERRAIN_ABOVE_DETAIL2,
					PROPERTY_ID_TERRAIN_ABOVE_DETAIL_NOISE,
					PROPERTY_ID_TERRAIN_BELOW,
					PROPERTY_ID_TERRAIN_BEACH2
				};

				for (uint32_t propertyId : testPropertyIds)
					pTerrainScript->SetProperty(propertyId, &Property().SetValueKey(key));

				//update current textures
				//cTerrainStateMgr_UpdateFromDefinition
				CALL(
					Address(ModAPI::ChooseAddress(0xf902d0, 0xfbc100)),
					void,
					Args(cTerrainStateMgr*, PropertyList*),
					Args(mpScenarioTerraformMode->mpTerrainStateMgr, pTerrainScript.get())
				);
#pragma endregion
				SelectItem(pItem);
				UpdatePaletteTexture();
				mpScenarioTerraformMode->CommitHistoryEntry();
			}
			return true;
		}
		case CONTROL_ID_CUSTOMIZATION_PANEL_SEARCHBOX_BTN_CLEAR:
		{
			ClearSearchbar();
			return true;
		}
		default:
			return false;
		}
	}
	case kMsgTextChanged:
	{
		if (message.source->GetControlID() == CONTROL_ID_CUSTOMIZATION_PANEL_SEARCHBOX)
		{
			if (!mpSearchboxTextEdit)
				return false;
			string16 searchString = mpSearchboxTextEdit->GetText();
			searchString.make_lower();
			if (IWindowPtr pSearchboxClearWin = message.source->GetParent()->
				FindWindowByID(CONTROL_ID_CUSTOMIZATION_PANEL_SEARCHBOX_BTN_CLEAR))
				pSearchboxClearWin->SetVisible(!searchString.empty());
			InitItems(
				mpContentClientWin.get(),
				mItemsGroup,
				//TODO: PLACEHOLDER
				PROPERTY_ID_TERRAIN_ABOVE_DETAIL2,
				searchString
			);
			return true;
		}
		return false;
	}
	default:
		return false;
	}
}