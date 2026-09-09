#pragma once

#include "ScenarioCustomizationItem.h"
#include <Spore\UTFWin\IText.h>

#define ScenarioCustomizationPtr intrusive_ptr<ScenarioCustomization>

// To avoid repeating UTFWin:: all the time.
using namespace UTFWin;

class ScenarioCustomization 
	: public IWinProc
	, public DefaultRefCounted
{
public:
	enum CustomizationItemsGroup : uint32_t
	{
		kCustomizationItemsGroupNone,
		kCustomizationItemsGroupTextures = GROUP_ID_TEXTURES_DEFINITIONS,
		kCustomizationItemsGroupEffects = GROUP_ID_EFFECTS_DEFINITIONS
	};

	enum CustomizationPropertyTexture : uint32_t
	{
		kCustomizationPropertyTextureDetail = PROPERTY_ID_TERRAIN_ABOVE_DETAIL2,
		kCustomizationPropertyTextureDetailNoise = PROPERTY_ID_TERRAIN_ABOVE_DETAIL_NOISE,
		kCustomizationPropertyTextureSeabed = PROPERTY_ID_TERRAIN_BELOW,
		kCustomizationPropertyTextureCliff = PROPERTY_ID_TERRAIN_CLIFF,
		kCustomizationPropertyTextureBeach = PROPERTY_ID_TERRAIN_BEACH2
	};
	enum CustomizationPropertyEffect : uint32_t
	{
		kCustomizationPropertyEffectGround = PROPERTY_ID_TERRAIN_PLAYER_GROUND_EFFECTS,
		kCustomizationPropertyEffectVisualStyle = PROPERTY_ID_VISUAL_STYLE
	};

	struct CustomizationItemsLookup
	{
		CustomizationItemsGroup mItemsGroup;
		uint32_t mPropertyId;
		string16 mSearchString;
		bool mbFillPanelWin;
		bool mbIgnoreBlacklists;

		CustomizationItemsLookup(
			CustomizationItemsGroup itemsGroup,
			uint32_t propertyId,
			string16 searchString = u"",
			bool bFillPanelWin = true,
			bool bIgnoreBlacklists = false
		)
			: mItemsGroup(itemsGroup)
			, mPropertyId(propertyId)
			, mSearchString(move(searchString))
			, mbFillPanelWin(bFillPanelWin)
			, mbIgnoreBlacklists(bIgnoreBlacklists)
		{ }

		CustomizationItemsLookup(
			CustomizationItemsGroup itemsGroup,
			uint32_t propertyId,
			bool bFillPanelWin,
			bool bIgnoreBlacklists
		)
			: CustomizationItemsLookup(itemsGroup, propertyId, u"", bFillPanelWin, bIgnoreBlacklists)
		{ }

		inline bool operator==(const CustomizationItemsLookup& b) const
		{
			return mItemsGroup == b.mItemsGroup &&
				mPropertyId == b.mPropertyId &&
				mSearchString == b.mSearchString &&
				mbFillPanelWin == b.mbFillPanelWin &&
				mbIgnoreBlacklists == b.mbIgnoreBlacklists;
		}

		inline bool operator!=(const CustomizationItemsLookup& b) const
		{
			return !(*this == b);
		}
	};

private:
#pragma region Constants
	static constexpr float GRID_START_X = 0.0f;
	static constexpr float GRID_START_Y = 0.0f;
	static constexpr float ITEM_MARGIN = 4.0f;
	static constexpr float ITEM_WIDTH = 64.0f + ITEM_MARGIN;
	static constexpr float ITEM_HEIGHT = 64.0f + ITEM_MARGIN;

	static const CustomizationPropertyTexture PROPERTY_DEFAULT_TEXTURE =
		kCustomizationPropertyTextureDetail;

	static const CustomizationPropertyEffect PROPERTY_DEFAULT_EFFECT =
		kCustomizationPropertyEffectVisualStyle;

	static const uint32_t CONTROL_ID_PALETTE_BLOCK_TEXTURE = id("PlanetCustomizationTextureBlock");
	static const uint32_t CONTROL_ID_PALETTE_BTN_TEXTURE = id("PlanetCustomizationSelectTexture");
	static const uint32_t CONTROL_ID_PALETTE_BTN_TEXTURE_THUMBNAIL = id("PlanetCustomizationTextureThumbnail");
	static const uint32_t CONTROL_ID_PALETTE_NAME_TEXTURE = id("PlanetCustomizationTextureName");
	static const uint32_t CONTROL_ID_PALETTE_PROPERTIES_TEXTURE = id("PlanetCustomizationTextureSelectProperty");
	static const uint32_t CONTROL_ID_PALETTE_BLOCK_EFFECT = id("PlanetCustomizationEffectBlock");
	static const uint32_t CONTROL_ID_PALETTE_BTN_EFFECT = id("PlanetCustomizationSelectEffect");
	static const uint32_t CONTROL_ID_PALETTE_BTN_EFFECT_THUMBNAIL = id("PlanetCustomizationEffectThumbnail");
	static const uint32_t CONTROL_ID_PALETTE_BTN_EFFECT_ICON = id("PlanetCustomizationEffectIcon");
	static const uint32_t CONTROL_ID_PALETTE_NAME_EFFECT = id("PlanetCustomizationEffectName");
	static const uint32_t CONTROL_ID_PALETTE_PROPERTIES_EFFECT = id("PlanetCustomizationEffectSelectProperty");
	static const uint32_t CONTROL_ID_PALETTE_CLEAR_EFFECT = id("PlanetCustomizationEffectClear");
	static const uint32_t CONTROL_ID_CUSTOMIZATION_PANEL_CLOSE = id("PlanetCustomizationClosePanel");
	static const uint32_t CONTROL_ID_CUSTOMIZATION_PANEL_SEARCHBOX = id("PlanetCustomizationItemsFilterTE");
	static const uint32_t CONTROL_ID_CUSTOMIZATION_PANEL_SEARCHBOX_BTN_CLEAR = id("PlanetCustomizationItemsFilterClear");

	static const uint32_t SOUND_ID_EDITOR_CLICK = id("editor_click");
	static const uint32_t SOUND_ID_EDITOR_CLICK_GENERAL = id("editor_general_click");
	static const uint32_t SOUND_ID_EDITOR_TRASH = id("editor_trash");
	static const uint32_t SOUND_ID_CUSTOMIZATION_PANEL_OPEN = id("editor_load");
	static const uint32_t SOUND_ID_CUSTOMIZATION_PANEL_CLOSE = id("editor_picture_close");
#pragma endregion

protected:
	Simulator::cScenarioTerraformMode* mpScenarioTerraformMode;

	IWindow* mpPaletteCategoryWin;
	IWindow* mpPanelWin;
	IWindow* mpScrollFrameVerticalWin;
	IWindow* mpContentClientWin;
	IWindow* mpSearchboxWin;
	ITextEdit* mpSearchboxTextEdit;
	IWindow* mpSearchboxClearWin;

	IWindow* mpPaletteTextureBlockWin;
	IWindow* mpPaletteTextureWin;
	IWindow* mpPaletteTextureThumbnailWin;
	IWindow* mpPaletteTextureNameWin;

	IWindow* mpPaletteEffectBlockWin;
	IWindow* mpPaletteEffectWin;
	IWindow* mpPaletteEffectThumbnailWin;
	IWindow* mpPaletteEffectIconWin;
	IWindow* mpPaletteEffectNameWin;
	IWindow* mpPaletteEffectClearWin;

	CustomizationPropertyTexture mPropertyTexture;
	CustomizationPropertyEffect mPropertyEffect;

	float mfPanelTextureY;
	float mfPanelEffectY;
	int mnColumns;

	map<IWindow*, ScenarioCustomizationItemPtr> mWinItemMap;
	ScenarioCustomizationItemPtr mpSelectedItem;
	CustomizationItemsLookup mLastLookup;
	bool mbIsPanelShown;

public:
	static const uint32_t TYPE = id(PrivateName("ScenarioCustomization"));
	
	ScenarioCustomization(
		IWindow* pPaletteWin,
		IWindow* pScrollFrameVerticalWin,
		IWindow* pContentClientWin
	);
	~ScenarioCustomization();

	ResourceKey GetCurrentCustomizationKey(uint32_t propertyId);

	void InitTextures(bool bFillPanelWin = true);
	void InitEffects(bool bFillPanelWin = true);
	inline void Init(bool bFillPanelWin = true)
	{
		InitTextures(bFillPanelWin);
		InitEffects(bFillPanelWin);
	}
	inline void SelectItem(ScenarioCustomizationItemPtr pItem)
	{
		if (mpSelectedItem)
			mpSelectedItem->SetSelection(false);
		if (pItem)
			pItem->SetSelection(true);
		mpSelectedItem = pItem;
	}
	inline void SwitchPaletteCategory() { HideCustomizationPanel(); }
	inline IWindow* GetCategoryWindow() { return mpPaletteCategoryWin; }

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	
	int GetEventFlags() const override;

	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;

protected:
	void InitItems(CustomizationItemsLookup itemsLookup);
	void ClearItems();
	void UpdatePaletteTexture();
	void UpdatePaletteEffect();
	void ShowCustomizationPanel(CustomizationItemsLookup itemsLookup, float positionY);
	void HideCustomizationPanel(bool bSilent = false);
	void CenterUIItem(IWindow* pChildWin);
	bool CheckPropertyWhitelistFromDefinition(
		uint32_t propertyId,
		PropertyListPtr pDefinitionPropList
	);
	bool CheckPropertyBlacklistFromDefinition(
		uint32_t propertyId,
		PropertyListPtr pDefinitionPropList
	);
	void UpdateSelectedCaption(IWindow* pCaptionWin, string16 pName);
	string16 ResourceKeyToString(ResourceKey key);
	void SetProperty(uint32_t propertyId, ResourceKey customizationKey);

	inline bool IsPropertyAllowedForDefinition(
		uint32_t propertyId,
		PropertyListPtr pDefinitionPropList
	)
	{
		return CheckPropertyWhitelistFromDefinition(propertyId, pDefinitionPropList) &&
			CheckPropertyBlacklistFromDefinition(propertyId, pDefinitionPropList);
	}
	inline void ClearSearchbar()
	{
		if (mpSearchboxTextEdit)
			mpSearchboxTextEdit->SetText(u"", 0);
	}

};
