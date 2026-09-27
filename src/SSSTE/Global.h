#pragma once

#pragma region Macros
// Scenario Share-Safe Textures & Effects
#define ModID "SSSTE"
#define PrivateName(name) (ModID "-" name)

#define EmptyKey ResourceKey(0, 0, 0)
#define EmDashString u"\u2014"
#define HexFormatString u"0x%08x"

#define LookupWindow(pWin, controlId) pWin ? pWin->FindWindowByID(controlId) : nullptr
#define Attach(pWin) if (pWin) pWin->AddWinProc(this)
#pragma endregion


#pragma region Externs
class ScenarioCustomization;
class ScenarioFloraGroundCoverLock;

extern int g_nScenarioCustomizationCategoryIndex;
extern intrusive_ptr<ScenarioCustomization> g_pWinProc;
extern intrusive_ptr<ScenarioFloraGroundCoverLock> g_pFloraGroundCoverLock;
#pragma endregion


#pragma region Constants
static const uint32_t PROPERTY_ID_TERRAIN_ABOVE_DETAIL2 = 0x3b4f7c9; // terrainThemeAboveDetail2
static const uint32_t PROPERTY_ID_TERRAIN_ABOVE_DETAIL_NOISE = 0x3b4f7ca; // terrainThemeAboveDetailNoise
static const uint32_t PROPERTY_ID_TERRAIN_BELOW = 0x3b4f7cb; // terrainThemeBelow
static const uint32_t PROPERTY_ID_TERRAIN_CLIFF = 0x3b4f7c6; // terrainThemeCliff
static const uint32_t PROPERTY_ID_TERRAIN_BEACH2 = 0x3b4f7cd; // terrainThemeBeach2
static const uint32_t PROPERTY_ID_TERRAIN_PLAYER_GROUND_EFFECTS = id("groundCoverLiningsGUIDs"); // terrainPlayerGroundEffects
static const uint32_t PROPERTY_ID_VISUAL_STYLE = id("visualStyle"); // visualStyle

static const uint32_t PROPERTY_ID_CUSTOMIZATION_ITEM_KEY = id("CustomizationItemKey");
static const uint32_t PROPERTY_ID_CUSTOMIZATION_ITEM_NAME = id("CustomizationItemName");
static const uint32_t PROPERTY_ID_CUSTOMIZATION_ITEM_WHITELIST = id("CustomizationItemPropertyWhitelist");
static const uint32_t PROPERTY_ID_CUSTOMIZATION_ITEM_BLACKLIST = id("CustomizationItemPropertyBlacklist");
static const uint32_t PROPERTY_ID_CUSTOMIZATION_ITEM_IS_GROUND_COVER = id("CustomizationItemIsGroundCover");

static const uint32_t PROPERTY_ID_PALETTE_CATEGORY_ICON_LIST_MODAPI = id("paletteCategoryIconListModAPI");

static const uint32_t GROUP_ID_TEXTURES_DEFINITIONS = 0x408a2100; // scenario_texture~
static const uint32_t GROUP_ID_EFFECTS_DEFINITIONS = 0x408a6f00; // scenario_filter~

static const uint32_t TABLE_ID_SAVE_AREAS = id(ModID);

static const uint32_t CONTROL_ID_PALETTE = id("PlanetCustomizationPalette");
static const uint32_t CONTROL_ID_MODAPI_HIDE = id("ModAPIHide"); // Error
static const uint32_t CONTROL_ID_MODAPI_SHOW = id("ModAPIShow"); // Actual content
static const uint32_t CONTROL_ID_CUSTOMIZATION_PANEL = id("PlanetCustomizationPanel");
static const uint32_t CONTROL_ID_CUSTOMIZATION_PANEL_ITEMS = id("PlanetCustomizationItems");
static const uint32_t CONTROL_ID_CUSTOMIZATION_ITEM = id("PlanetCustomizationItem");
static const uint32_t CONTROL_ID_PALETTE_FLORA_GROUND_COVER = 0x7d61ff2;
#pragma endregion
