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

		CustomizationItemsLookup(
			CustomizationItemsGroup itemsGroup,
			uint32_t propertyId,
			string16 searchString = u""
		)
			: mItemsGroup(itemsGroup)
			, mPropertyId(propertyId)
			, mSearchString(searchString)
		{ }

		inline bool operator==(const CustomizationItemsLookup& b) const
		{
			return mItemsGroup == b.mItemsGroup &&
				mPropertyId == b.mPropertyId &&
				mSearchString == b.mSearchString;
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
	static constexpr float ITEM_MARGIN = 4.0f; // TBD
	static constexpr float ITEM_WIDTH = 64.0f + ITEM_MARGIN; // TBD
	static constexpr float ITEM_HEIGHT = 64.0f + ITEM_MARGIN; // TBD

	static const CustomizationPropertyTexture PROPERTY_DEFAULT_TEXTURE =
		kCustomizationPropertyTextureDetail;

	static const CustomizationPropertyEffect PROPERTY_DEFAULT_EFFECT =
		kCustomizationPropertyEffectGround;
#pragma endregion

protected:
	IWindow* mpPaletteCategoryWin;
	IWindow* mpPanelWin;
	IWindow* mpScrollFrameVerticalWin;
	IWindow* mpContentClientWin;
	ITextEdit* mpSearchboxTextEdit;
	IWindow* mpPaletteTextureBlockWin;
	IWindow* mpPaletteTextureWin;
	IWindow* mpPaletteTextureNameWin;
	//IWindow* mpPaletteEffectBlockWin;
	//IWindow* mpPaletteEffectWin;
	//IText* mpPaletteEffectName;
	float mPanelTextureY;
	//float mPanelEffectY;
	Simulator::cScenarioTerraformMode* mpScenarioTerraformMode;
	vector<ScenarioCustomizationItemPtr> mItems;
	map<IWindow*, ScenarioCustomizationItemPtr> mWinItemMap;
	ScenarioCustomizationItemPtr mpSelectedItem;
	CustomizationItemsLookup mLastLookup;
	CustomizationPropertyTexture mPropertyTexture;
	CustomizationPropertyEffect mPropertyEffect;
	bool mbIsPanelShown;

public:
	static const uint32_t TYPE = id(PrivateName("ScenarioCustomization"));
	
	ScenarioCustomization(
		IWindow* pPaletteWin,
		IWindow* pScrollFrameVerticalWin,
		IWindow* pContentClientWin
	);
	~ScenarioCustomization();

	void InitItems(IWindow* pWindow, CustomizationItemsLookup itemsLookup);
	void ClearItems();

	inline void SelectItem(ScenarioCustomizationItemPtr pItem)
	{
		if (mpSelectedItem)
			mpSelectedItem->SetSelection(false);
		if (pItem)
			pItem->SetSelection(true);
		mpSelectedItem = pItem;
	}
	inline ResourceKey GetCurrentCustomizationKey(uint32_t propertyId)
	{
		ResourceKey customizationKeyCurrent;
		return (mpScenarioTerraformMode && App::Property::GetKey(
			mpScenarioTerraformMode->mpPropList.get(),
			propertyId,
			customizationKeyCurrent
		))
			? customizationKeyCurrent
			: EmptyKey;
	}
	inline void SwitchPaletteCategory() { HideCustomizationPanel(); }

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	
	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;

protected:
	void UpdatePaletteTexture();
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
