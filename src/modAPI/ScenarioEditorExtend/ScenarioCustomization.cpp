#include "stdafx.h"
#include "Global.h"
#include "ScenarioCustomization.h"
#include "ScenarioFloraGroundCoverLock.h"
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
	, mpPanelWin(LookupWindow(mpPaletteCategoryWin, CONTROL_ID_CUSTOMIZATION_PANEL))
	, mpScrollFrameVerticalWin(pScrollFrameVerticalWin)
	, mpContentClientWin(pContentClientWin)
	, mpSearchboxWin(LookupWindow(mpPanelWin, CONTROL_ID_CUSTOMIZATION_PANEL_SEARCHBOX))
	, mpSearchboxTextEdit(mpSearchboxWin
		? (ITextEdit*)mpSearchboxWin->Cast(ITextEdit::TYPE)
		: nullptr
	)
	, mpSearchboxClearWin(LookupWindow(
		mpPanelWin,
		CONTROL_ID_CUSTOMIZATION_PANEL_SEARCHBOX_BTN_CLEAR)
	)
	, mpPaletteTextureBlockWin(LookupWindow(mpPaletteCategoryWin, CONTROL_ID_PALETTE_BLOCK_TEXTURE))
	, mpPaletteTextureWin(LookupWindow(mpPaletteTextureBlockWin, CONTROL_ID_PALETTE_BTN_TEXTURE))
	, mpPaletteTextureThumbnailWin(LookupWindow(
		mpPaletteTextureWin,
		CONTROL_ID_PALETTE_BTN_TEXTURE_THUMBNAIL)
	)
	, mpPaletteTextureNameWin(LookupWindow(
		mpPaletteTextureBlockWin,
		CONTROL_ID_PALETTE_NAME_TEXTURE)
	)
	, mpPaletteEffectBlockWin(LookupWindow(mpPaletteCategoryWin,CONTROL_ID_PALETTE_BLOCK_EFFECT))
	, mpPaletteEffectWin(LookupWindow(mpPaletteEffectBlockWin, CONTROL_ID_PALETTE_BTN_EFFECT))
	, mpPaletteEffectThumbnailWin(LookupWindow(
		mpPaletteEffectWin,
		CONTROL_ID_PALETTE_BTN_EFFECT_THUMBNAIL)
	)
	, mpPaletteEffectIconWin(LookupWindow(
		mpPaletteEffectThumbnailWin,
		CONTROL_ID_PALETTE_BTN_EFFECT_ICON)
	)
	, mpPaletteEffectNameWin(LookupWindow(mpPaletteEffectBlockWin, CONTROL_ID_PALETTE_NAME_EFFECT))
	, mpPaletteEffectClearWin(LookupWindow(
		mpPaletteEffectBlockWin,
		CONTROL_ID_PALETTE_CLEAR_EFFECT)
	)
	, mPropertyTexture(PROPERTY_DEFAULT_TEXTURE)
	, mPropertyEffect(PROPERTY_DEFAULT_EFFECT)
	, mPanelTextureY(mpPaletteTextureBlockWin
		? mpPaletteTextureBlockWin->GetRealArea().top
		: 0.0f
	)
	, mPanelEffectY(mpPaletteEffectBlockWin
		? mpPaletteEffectBlockWin->GetRealArea().top
		: 0.0f
	)
	, mColumns(0)
	, mpSelectedItem(nullptr)
	, mLastLookup(kCustomizationItemsGroupNone, 0)
	, mbIsPanelShown(false)
{
	if (mpContentClientWin)
	{
		Math::Rectangle areaContentClientWin = mpContentClientWin->GetRealArea();
		mColumns = (int)((areaContentClientWin.GetWidth() + ITEM_MARGIN) / ITEM_WIDTH);
		if (mColumns < 1)
			mColumns = 1;
	}

	Attach(mpPaletteTextureWin);
	if (mpPaletteTextureWin)
	{
		CenterUIItem(mpPaletteCategoryWin->FindWindowByID(CONTROL_ID_PALETTE_PROPERTIES_TEXTURE));
		InitTextures(false);
	}
	Attach(mpPaletteEffectWin);
	if (mpPaletteEffectWin)
		InitEffects(false);

	Attach(mpPanelWin);
	Attach(mpSearchboxWin);
	IWindow* pPanelCloseWin = LookupWindow(mpPanelWin, CONTROL_ID_CUSTOMIZATION_PANEL_CLOSE);
	Attach(pPanelCloseWin);
	IWindow* pPaletteTexturePropertySelectWin = LookupWindow(
		mpPaletteTextureBlockWin,
		CONTROL_ID_PALETTE_PROPERTIES_TEXTURE
	);
	Attach(pPaletteTexturePropertySelectWin);
	IWindow* pPaletteEffectPropertySelectWin = LookupWindow(
		mpPaletteEffectBlockWin,
		CONTROL_ID_PALETTE_PROPERTIES_EFFECT
	);
	Attach(pPaletteEffectPropertySelectWin);
	Attach(mpPaletteEffectClearWin);
}

ScenarioCustomization::~ScenarioCustomization()
{
}

void ScenarioCustomization::InitItems(
	CustomizationItemsLookup itemsLookup,
	bool bFillPanelWin,
	bool bIgnoreBlacklists
)
{
	if ((mLastLookup != itemsLookup || !mWinItemMap.size()) && mpContentClientWin && mColumns)
	{
		ClearItems();
		mpSelectedItem = nullptr;
		ResourceKey customizationKeyCurrent = GetCurrentCustomizationKey(itemsLookup.mPropertyId);
		vector<uint32_t> definitionIds;
		PropManager.GetPropertyListIDs(itemsLookup.mItemsGroup, definitionIds);
		if (!definitionIds.empty())
		{
			vector<ScenarioCustomizationItemPtr> items;
			for (uint32_t definitionId : definitionIds)
			{
				PropertyListPtr pDefinitionPropList;
				if (!PropManager.GetPropertyList(
					definitionId,
					itemsLookup.mItemsGroup,
					pDefinitionPropList)
				)
					continue;

				if (!bIgnoreBlacklists &&
					!IsPropertyAllowedForDefinition(itemsLookup.mPropertyId, pDefinitionPropList)
				)
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
					if (IWindow* pItemWin = pItem->FindWindowByID(CONTROL_ID_CUSTOMIZATION_ITEM))
						pItemWin->SetVisible(false);
					mpSelectedItem = pItem;
				}
				items.push_back(pItem);
#pragma endregion

			}

			if (!bFillPanelWin)
			{
				mLastLookup = itemsLookup;
				return;
			}

#pragma region Sort
			stable_sort(
				items.begin(),
				items.end(),
				[](
					const ScenarioCustomizationItemPtr& a,
					const ScenarioCustomizationItemPtr& b
				)
				{ return a->GetCustomization() < b->GetCustomization(); }
			);
			items.erase(
				unique(
					items.rbegin(),
					items.rend(),
					[](
						const ScenarioCustomizationItemPtr& a,
						const ScenarioCustomizationItemPtr& b
					)
					{ return a->GetCustomization() == b->GetCustomization(); }
				),
				items.rend()
			);

			quick_sort(
				items.begin(),
				items.end(),
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
			if (mColumns)
				for (ScenarioCustomizationItemPtr pItem : items)
				{
					pItem->SetParentWindow(mpContentClientWin);
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
						if (pItem == mpSelectedItem)
							pItemWin->SetVisible(true);
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
		mpContentClientWin->RemoveWindow(pairWinItem.first);
	mWinItemMap.clear();
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

ResourceKey ScenarioCustomization::GetCurrentCustomizationKey(uint32_t propertyId)
{
	if (!mpScenarioTerraformMode)
		return EmptyKey;
	ResourceKey customizationKeyCurrent;
	PropertyList* pPropList = mpScenarioTerraformMode->mpPropList.get();
	if (!Property::GetKey(pPropList, propertyId, customizationKeyCurrent))
		Property::GetUInt32(pPropList, propertyId, customizationKeyCurrent.instanceID);
	return customizationKeyCurrent;
}

void ScenarioCustomization::UpdatePaletteTexture()
{
	if (mpPaletteTextureThumbnailWin)
	{
		ResourceKey customizationKeyCurrent = mpSelectedItem
			? mpSelectedItem->GetThumbnail()
			: GetCurrentCustomizationKey(mPropertyTexture);
		if (!customizationKeyCurrent.typeID)
			customizationKeyCurrent.typeID = TypeIDs::rw4;
		Image::SetBackgroundByKey(mpPaletteTextureThumbnailWin, customizationKeyCurrent);
	}
	UpdateSelectedCaption(mpPaletteTextureNameWin, mpSelectedItem
		? mpSelectedItem->GetName()->GetText()
		: ResourceKeyToString(GetCurrentCustomizationKey(mPropertyTexture))
	);
}

// TODO: simplify (merge with UpdatePaletteTexture() somehow???)
void ScenarioCustomization::UpdatePaletteEffect()
{
	ResourceKey customizationKeyCurrent = mpSelectedItem
		? mpSelectedItem->GetThumbnail()
		: GetCurrentCustomizationKey(mPropertyEffect);
	bool bIsCustomized = customizationKeyCurrent != EmptyKey;
	mpPaletteEffectClearWin->SetEnabled(bIsCustomized);
	if (mpPaletteEffectThumbnailWin)
	{
		if (mpPaletteEffectIconWin)
			mpPaletteEffectIconWin->SetVisible(bIsCustomized);
		Image::SetBackgroundByKey(mpPaletteEffectThumbnailWin, customizationKeyCurrent);
	}
	UpdateSelectedCaption(mpPaletteEffectNameWin, mpSelectedItem
		? mpSelectedItem->GetName()->GetText()
		: ResourceKeyToString(instance_id(GetCurrentCustomizationKey(mPropertyEffect).instanceID))
	);
	if (mPropertyEffect == kCustomizationPropertyEffectGround)
		g_pFloraGroundCoverLock->SetLock(mpSelectedItem);
}

void ScenarioCustomization::UpdateSelectedCaption(IWindow* pCaptionWin, string16 pName)
{
	if (!pCaptionWin)
		return;
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

void ScenarioCustomization::CenterUIItem(IWindow* pChildWin)
{
	if (!pChildWin)
		return;
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

#pragma region Show & Hide Panel
void ScenarioCustomization::ShowCustomizationPanel(
	CustomizationItemsLookup itemsLookup,
	float positionY
)
{
	ClearSearchbar();
	InitItems(itemsLookup);

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

void ScenarioCustomization::SetProperty(uint32_t propertyId, ResourceKey customizationKey)
{
	switch (propertyId)
	{
	case kCustomizationPropertyEffectVisualStyle:
		CALL(
			GetAddress(SSSTE::cScenarioTerraformMode, SetVisualStyle),
			void,
			Args(cScenarioTerraformMode*, ResourceKey*),
			Args(mpScenarioTerraformMode, &customizationKey)
		);
		break;
	case kCustomizationPropertyEffectGround:
		CALL(
			GetAddress(SSSTE::cScenarioTerraformMode, SetGroundEffectId),
			void,
			Args(cScenarioTerraformMode*, uint32_t),
			Args(mpScenarioTerraformMode, customizationKey.instanceID)
		);
		STATIC_CALL_(GetAddress(SSSTE::cScenarioTerraformMode, ReloadGroundEffect), void);
		break;
	default:
		PropertyListPtr pTerrainScript = mpScenarioTerraformMode->mpPropList;
		pTerrainScript->SetProperty(
			propertyId,
			&Property().SetValueKey(customizationKey)
		);
		PlanetModel.mpSphere->ParseProp(pTerrainScript.get());
		break;
	}
}

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
		case CONTROL_ID_PALETTE_BTN_TEXTURE:
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
		case CONTROL_ID_PALETTE_BTN_EFFECT:
		{
			if (mbIsPanelShown && mLastLookup.mItemsGroup == kCustomizationItemsGroupEffects)
				HideCustomizationPanel();
			else
			{
				ShowCustomizationPanel(
					CustomizationItemsLookup(kCustomizationItemsGroupEffects, mPropertyEffect),
					mPanelEffectY
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
			ResourceKey customizationKey = pItem->GetCustomization();

			mpScenarioTerraformMode->StartHistoryEntry();
			SetProperty(mLastLookup.mPropertyId, customizationKey);
			mpScenarioTerraformMode->CommitHistoryEntry();

			SelectItem(pItem);
			switch (mLastLookup.mItemsGroup)
			{
			case kCustomizationItemsGroupTextures:
				UpdatePaletteTexture();
				break;
			case kCustomizationItemsGroupEffects:
				UpdatePaletteEffect();
				break;
			}
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
		case CONTROL_ID_PALETTE_CLEAR_EFFECT:
		{
			ResourceKey customizationKey = GetCurrentCustomizationKey(mPropertyEffect);
			if (customizationKey == EmptyKey)
				return true;

			mpScenarioTerraformMode->StartHistoryEntry();
			SetProperty(mPropertyEffect, EmptyKey);
			mpScenarioTerraformMode->CommitHistoryEntry();

			if (mpSelectedItem &&
				(mLastLookup.mPropertyId != mPropertyEffect ||
					mpSelectedItem->GetCustomization() == customizationKey
				)
			)
				SelectItem(nullptr);

			if (mPropertyEffect == kCustomizationPropertyEffectGround && g_pFloraGroundCoverLock)
				g_pFloraGroundCoverLock->Unlock();
			UpdatePaletteEffect();
			PlayAudio(SOUND_ID_EDITOR_TRASH);
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
		if (!pParentWin)
			return false;

		switch (pParentWin->GetControlID())
		{
		case CONTROL_ID_PALETTE_PROPERTIES_TEXTURE:
			HideCustomizationPanel(controlId != mPropertyTexture);
			if (controlId == mPropertyTexture)
				return true;
			mPropertyTexture = (CustomizationPropertyTexture)controlId;
			InitTextures(false);
			break;
		case CONTROL_ID_PALETTE_PROPERTIES_EFFECT:
			HideCustomizationPanel(controlId != mPropertyEffect);
			if (controlId == mPropertyEffect)
				return true;
			mPropertyEffect = (CustomizationPropertyEffect)controlId;
			InitEffects(false);
		default:
			return false;
		}
		PlayAudio(SOUND_ID_EDITOR_CLICK);
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
			if (mpSearchboxClearWin)
				mpSearchboxClearWin->SetVisible(!searchString.empty());
			InitItems({ mLastLookup.mItemsGroup, mLastLookup.mPropertyId, searchString });
			return true;
		}
		return false;
	}
	default:
		return false;
	}
}
