#include "stdafx.h"
#include "Global.h"
#include "ScenarioCustomization.h"
#include <EASTL\sort.h>
#include <Spore\UI\ScrollFrameVertical.h>

using namespace App;
using namespace Audio;
using namespace Simulator;
using namespace Terrain;
using namespace UI;

ScenarioCustomization::ScenarioCustomization(
	IWindow* pPaletteCategoryWin,
	IWindow* pScrollFrameVerticalWin,
	IWindow* pContentClientWin
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
	, mbIsPanelShown(false)
{
	if (mpPaletteTextureWin)
	{
		CenterUIItem(mpPaletteCategoryWin->FindWindowByID(CONTROL_ID_PALETTE_PROPERTIES_TEXTURE));
		InitItems(
			mpContentClientWin,
			{ kCustomizationItemsGroupTextures, PROPERTY_DEFAULT_TEXTURE }
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
		int ñolumns = (int)((winWidth + ITEM_MARGIN) / ITEM_WIDTH);
		if (ñolumns < 1)
			ñolumns = 1;

		ClearItems();
		mpSelectedItem = nullptr;
		ResourceKey customizationKeyCurrent = GetCurrentCustomizationKey(itemsLookup.mPropertyId);
		vector<uint32_t> definitionIds;
		PropManager.GetPropertyListIDs(itemsLookup.mItemsGroup, definitionIds);
		if (!definitionIds.empty())
		{
			for (uint32_t definitionId : definitionIds)
			{
				PropertyListPtr pDefinitionPropList;
				if (!PropManager.GetPropertyList(
					definitionId,
					itemsLookup.mItemsGroup,
					pDefinitionPropList)
				)
					continue;

				if (!IsPropertyAllowedForDefinition(itemsLookup.mPropertyId, pDefinitionPropList))
					continue;

#pragma region Add Item
				ScenarioCustomizationItemPtr pItem = new ScenarioCustomizationItem();
				pItem->SetCustomizationAndImage(
					pDefinitionPropList.get(),
					ResourceKey(definitionId, TypeIDs::png, itemsLookup.mItemsGroup),
					this
				);
				string16 itemName = pItem->GetName()->GetText();
				itemName.make_lower();
				if (itemName.find(itemsLookup.mSearchString) == string16::npos)
					continue;
				if (pItem->GetCustomization() == customizationKeyCurrent)
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
				{ return a->GetCustomization() < b->GetCustomization(); }
			);
			mItems.erase(
				unique(
					mItems.rbegin(),
					mItems.rend(),
					[](
						const ScenarioCustomizationItemPtr& a,
						const ScenarioCustomizationItemPtr& b
					)
					{ return a->GetCustomization() == b->GetCustomization(); }
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
				int row = index / ñolumns;
				int col = index % ñolumns;
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

		ScrollFrameVertical::Update(mpScrollFrameVerticalWin);
		mLastLookup = itemsLookup;
	}
}

#pragma region Whitelist & Blacklist
bool ScenarioCustomization::CheckPropertyWhitelistFromDefinition(
	uint32_t propertyId,
	PropertyListPtr pDefinitionPropList
)
{
	vector<uint32_t> definitionPropertyWhitelist;
	Property::GetArrayUInt32(
		pDefinitionPropList.get(),
		PROPERTY_ID_CUSTOMIZATION_ITEM_WHITELIST,
		definitionPropertyWhitelist
	);
	return !definitionPropertyWhitelist.size() ||
		find(
			definitionPropertyWhitelist.begin(),
			definitionPropertyWhitelist.end(),
			propertyId
		) != definitionPropertyWhitelist.end();
}

bool ScenarioCustomization::CheckPropertyBlacklistFromDefinition(
	uint32_t propertyId,
	PropertyListPtr pDefinitionPropList
)
{
	vector<uint32_t> definitionPropertyBlacklist;
	Property::GetArrayUInt32(
		pDefinitionPropList.get(),
		PROPERTY_ID_CUSTOMIZATION_ITEM_BLACKLIST,
		definitionPropertyBlacklist
	);
	return find(
		definitionPropertyBlacklist.begin(),
		definitionPropertyBlacklist.end(),
		propertyId
	) == definitionPropertyBlacklist.end();
}
#pragma endregion

void ScenarioCustomization::ClearItems()
{
	for (pair<IWindow* const, ScenarioCustomizationItemPtr>& pairWinItem : mWinItemMap)
		pairWinItem.first->RemoveWindow(pairWinItem.first);
	mWinItemMap.clear();
	mItems.clear();
}

string16 ScenarioCustomization::ResourceKeyToString(ResourceKey key)
{
	string16 string;
	if (key == EmptyKey)
	{
		string.append(EmDashString);
		return string;
	}
	if (key.groupID)
		string.append_sprintf(HexFormatString u"!", key.groupID);
	string.append_sprintf(HexFormatString, key.instanceID);
	if (key.typeID)
		string.append_sprintf(u"." HexFormatString, key.typeID);
	return string;
}

void ScenarioCustomization::UpdatePaletteTexture()
{
	if (IWindow* pPaletteTextureThumbnailWin = mpPaletteTextureWin->
		FindWindowByID(CONTROL_ID_PALETTE_BTN_TEXTURE_THUMBNAIL))
	{
		ResourceKey customizationKeyCurrent = mpSelectedItem
			? mpSelectedItem->GetThumbnail()
			: GetCurrentCustomizationKey(mPropertyTexture);
		if (!customizationKeyCurrent.typeID)
			customizationKeyCurrent.typeID = TypeIDs::rw4;
		Image::SetBackgroundByKey(pPaletteTextureThumbnailWin, customizationKeyCurrent);
	}
	UpdateSelectedCaption(mpPaletteTextureNameWin, mpSelectedItem
		? mpSelectedItem->GetName()->GetText()
		: ResourceKeyToString(GetCurrentCustomizationKey(mPropertyTexture))
	);
}

void ScenarioCustomization::UpdateSelectedCaption(IWindow* pCaptionWin, string16 pName)
{
	if (pCaptionWin)
	{
		pCaptionWin->SetCaption(pName.data());
		if (IWindow* pCaptionChildWin = *pCaptionWin->children().begin())
			if (SporeTooltipWinProc* pTooltipWinProc = (SporeTooltipWinProc*)pCaptionChildWin->
				GetNextWinProc()->Cast(SporeTooltipWinProc::TYPE))
				if (pName != EmDashString)
					pTooltipWinProc->mText = pName;
				else
					pCaptionChildWin->RemoveWinProc(pTooltipWinProc);
			else
				pCaptionChildWin->AddWinProc(CreateTooltip(pName.data()));
	}
}

void ScenarioCustomization::CenterUIItem(IWindow* pChildWin)
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
		mpContentClientWin,
		itemsLookup
	);

	Math::Rectangle areaPanelWin = mpPanelWin->GetArea();
	float heightPanelWin = areaPanelWin.GetHeight();
	areaPanelWin.top = positionY;
	areaPanelWin.bottom = areaPanelWin.top + heightPanelWin;
	mpPanelWin->SetArea(areaPanelWin);

	mpPanelWin->SetVisible(true);
	mpContentClientWin->SetVisible(true);
	mbIsPanelShown = true;
	PlayAudio(SOUND_ID_CUSTOMIZATION_PANEL_OPEN);
}

void ScenarioCustomization::HideCustomizationPanel(bool bSilent)
{
	if (mbIsPanelShown)
	{
		if (!bSilent)
			PlayAudio(SOUND_ID_CUSTOMIZATION_PANEL_CLOSE);
		mpPanelWin->SetVisible(false);
		mpContentClientWin->SetVisible(false);
		mbIsPanelShown = false;
	}
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
			if (mbIsPanelShown && mLastLookup.mItemsGroup == kCustomizationItemsGroupTextures)
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
			ResourceKey customizationKey = pItem->GetCustomization();
			PropertyListPtr pTerrainScript = mpScenarioTerraformMode->mpPropList;

			pTerrainScript->SetProperty(
				mLastLookup.mPropertyId,
				&Property().SetValueKey(customizationKey)
			);

			//update current textures
			//cTerrainStateMgr_UpdateFromDefinition
			CALL(
				GetAddress(SSSTE::cTerrainStateMgr, UpdateFromDefinition),
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
		IWindow* pParentWin = message.source->GetParent();
		if (
			pParentWin &&
			pParentWin->GetControlID() == CONTROL_ID_PALETTE_PROPERTIES_TEXTURE
		)
		{
			HideCustomizationPanel(controlId != mPropertyTexture);
			if (controlId == mPropertyTexture)
				return true;
			mPropertyTexture = (CustomizationPropertyTexture)controlId;
			InitItems(
				mpContentClientWin,
				{ kCustomizationItemsGroupTextures, mPropertyTexture }
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
			InitItems(
				mpContentClientWin,
				{ mLastLookup.mItemsGroup, mLastLookup.mPropertyId, searchString }
			);
			return true;
		}
		return false;
	}
	default:
		return false;
	}
}
