#pragma once

#define ModID "SSSTE" // Scenario_ShareSafe_TexturesEffects
#define PrivateName(name) (ModID "-" name)

#define EmptyKey ResourceKey(0, 0, 0)

class ScenarioCustomization;

extern intrusive_ptr<ScenarioCustomization> g_pWinProc;

#pragma region constants
static const uint32_t PROPERTY_ID_TERRAIN_CLIFF = 0x3b4f7c6;
static const uint32_t PROPERTY_ID_TERRAIN_ABOVE_DETAIL2 = 0x3b4f7c9;
static const uint32_t PROPERTY_ID_TERRAIN_ABOVE_DETAIL_NOISE = 0x3b4f7ca;
static const uint32_t PROPERTY_ID_TERRAIN_BELOW = 0x3b4f7cb;
static const uint32_t PROPERTY_ID_TERRAIN_BEACH2 = 0x3b4f7cd;
static const uint32_t PROPERTY_ID_CUSTOMIZATION_ITEM_KEY = id("CustomizationItemKey");
static const uint32_t PROPERTY_ID_CUSTOMIZATION_ITEM_NAME = id("CustomizationItemName");
static const uint32_t PROPERTY_ID_CUSTOMIZATION_ITEM_BLACKLIST = id("CustomizationItemPropertyBlacklist");

static const uint32_t GROUP_ID_TEXTURES_DEFINITIONS = id("ScenarioCustomizationTextures");
static const uint32_t GROUP_ID_EFFECTS_DEFINITIONS = id("ScenarioCustomizationEffects");

static const uint32_t CONTROL_ID_CUSTOMIZATION_PANEL = id("PlanetCustomizationPanel");
static const uint32_t CONTROL_ID_CUSTOMIZATION_PANEL_BTN_TEXTURE = id("PlanetCustomizationChooseTexture");
static const uint32_t CONTROL_ID_CUSTOMIZATION_PANEL_BTN_EFFECT = id("PlanetCustomizationChooseEffect");
static const uint32_t CONTROL_ID_CUSTOMIZATION_PANEL_ITEMS = id("PlanetCustomizationItems");
static const uint32_t CONTROL_ID_CUSTOMIZATION_PANEL_SEARCHBOX = id("PlanetCustomizationItemsFilterTE");
static const uint32_t CONTROL_ID_CUSTOMIZATION_PANEL_SEARCHBOX_BTN_CLEAR = id("PlanetCustomizationItemsFilterClear");
static const uint32_t CONTROL_ID_CUSTOMIZATION_ITEM = id("PlanetCustomizationItem");
static const uint32_t CONTROL_ID_CUSTOMIZATION_ITEM_THUMBNAIL = id("PlanetCustomizationItemThumbnail");
static const uint32_t CONTROL_ID_CUSTOMIZATION_ITEM_CURSOR = id("PlanetCustomizationItemSelected");
#pragma endregion
