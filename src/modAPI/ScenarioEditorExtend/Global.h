#pragma once

#define ModID "SSSTE" // Scenario_ShareSafe_TexturesEffects
#define PrivateName(name) (ModID "-" name)

#define EmptyKey ResourceKey(0, 0, 0)
#define EmDashString u"\u2014"
#define HexFormatString u"0x%08x"

#define LookupWindow(pWin, controlId) pWin ? pWin->FindWindowByID(controlId) : nullptr
#define Attach(pWin) if (pWin) pWin->AddWinProc(this)

class ScenarioEditModeSculptFloraUI {};

class ScenarioCustomization;
class ScenarioFloraGroundCoverLock;

extern intrusive_ptr<ScenarioCustomization> g_pWinProc;
extern intrusive_ptr<ScenarioFloraGroundCoverLock> g_pFloraGroundCoverLock;

#pragma region Addresses
namespace SSSTE
{
	using namespace ModAPI;

	namespace Addresses(cScenarioTerraformMode)
	{
		DefineAddress(SetVisualStyle, ChooseAddress(0xf079d0, 0xf339f0));
		DefineAddress(SetGroundEffectId, ChooseAddress(0xf09140, 0xf35160));
		DefineAddress(ReloadGroundEffect, ChooseAddress(0xf05d70, 0xf31d40));
	}
	
	namespace Addresses(cScenarioEditModeDisplayStrategy)
	{
		DefineAddress(SetMode, ChooseAddress(0xeaab10, 0xed6620));
	}

	namespace Addresses(ScenarioEditModeSculptFloraUI)
	{
		DefineAddress(UpdateFloraCategoryUI, ChooseAddress(0xebd020, 0xee8c80));
	}
}
#pragma endregion

#pragma region Constants
static const uint32_t PROPERTY_ID_TERRAIN_ABOVE_DETAIL2 = 0x3b4f7c9;
static const uint32_t PROPERTY_ID_TERRAIN_ABOVE_DETAIL_NOISE = 0x3b4f7ca;
static const uint32_t PROPERTY_ID_TERRAIN_BELOW = 0x3b4f7cb;
static const uint32_t PROPERTY_ID_TERRAIN_CLIFF = 0x3b4f7c6;
static const uint32_t PROPERTY_ID_TERRAIN_BEACH2 = 0x3b4f7cd;
static const uint32_t PROPERTY_ID_TERRAIN_PLAYER_GROUND_EFFECTS = 0x56b14f05;
static const uint32_t PROPERTY_ID_VISUAL_STYLE = id("visualStyle");
static const uint32_t PROPERTY_ID_CUSTOMIZATION_ITEM_KEY = id("CustomizationItemKey");
static const uint32_t PROPERTY_ID_CUSTOMIZATION_ITEM_NAME = id("CustomizationItemName");
static const uint32_t PROPERTY_ID_CUSTOMIZATION_ITEM_WHITELIST = id("CustomizationItemPropertyWhitelist");
static const uint32_t PROPERTY_ID_CUSTOMIZATION_ITEM_BLACKLIST = id("CustomizationItemPropertyBlacklist");

static const uint32_t GROUP_ID_TEXTURES_DEFINITIONS = id("ScenarioCustomizationTextures");
static const uint32_t GROUP_ID_EFFECTS_DEFINITIONS = id("ScenarioCustomizationEffects");

static const uint32_t CONTROL_ID_PALETTE = id("PlanetCustomizationPalette");
static const uint32_t CONTROL_ID_PALETTE_BLOCK_TEXTURE = id("PlanetCustomizationTextureBlock");
static const uint32_t CONTROL_ID_PALETTE_BTN_TEXTURE = id("PlanetCustomizationSelectTexture");
static const uint32_t CONTROL_ID_PALETTE_BTN_TEXTURE_THUMBNAIL = id("PlanetCustomizationTextureThumbnail");
static const uint32_t CONTROL_ID_PALETTE_NAME_TEXTURE = id("PlanetCustomizationTextureName");
static const uint32_t CONTROL_ID_PALETTE_PROPERTIES_TEXTURE = id("PlanetCustomizationTextureSelectProperty");
static const uint32_t CONTROL_ID_PALETTE_BLOCK_EFFECT = id("PlanetCustomizationEffectBlock");
static const uint32_t CONTROL_ID_PALETTE_BTN_EFFECT = id("PlanetCustomizationSelectEffect");
static const uint32_t CONTROL_ID_PALETTE_BTN_EFFECT_THUMBNAIL = id("PlanetCustomizationEffectThumbnail");
static const uint32_t CONTROL_ID_PALETTE_NAME_EFFECT = id("PlanetCustomizationEffectName");
static const uint32_t CONTROL_ID_PALETTE_PROPERTIES_EFFECT = id("PlanetCustomizationEffectSelectProperty");
static const uint32_t CONTROL_ID_CUSTOMIZATION_PANEL = id("PlanetCustomizationPanel");
static const uint32_t CONTROL_ID_CUSTOMIZATION_PANEL_CLOSE = id("PlanetCustomizationClosePanel");
static const uint32_t CONTROL_ID_CUSTOMIZATION_PANEL_SEARCHBOX = id("PlanetCustomizationItemsFilterTE");
static const uint32_t CONTROL_ID_CUSTOMIZATION_PANEL_SEARCHBOX_BTN_CLEAR = id("PlanetCustomizationItemsFilterClear");
static const uint32_t CONTROL_ID_CUSTOMIZATION_PANEL_ITEMS = id("PlanetCustomizationItems");
static const uint32_t CONTROL_ID_CUSTOMIZATION_ITEM = id("PlanetCustomizationItem");
static const uint32_t CONTROL_ID_CUSTOMIZATION_ITEM_THUMBNAIL = id("PlanetCustomizationItemThumbnail");
static const uint32_t CONTROL_ID_CUSTOMIZATION_ITEM_CURSOR = id("PlanetCustomizationItemSelected");

static const uint32_t SOUND_ID_EDITOR_CLICK = id("editor_click");
static const uint32_t SOUND_ID_EDITOR_CLICK_GENERAL = id("editor_general_click");
static const uint32_t SOUND_ID_CUSTOMIZATION_PANEL_OPEN = id("editor_load");
static const uint32_t SOUND_ID_CUSTOMIZATION_PANEL_CLOSE = id("editor_picture_close");
#pragma endregion
