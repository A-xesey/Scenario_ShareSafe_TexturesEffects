#pragma once

namespace SSSTE
{
    using namespace ModAPI;

    namespace Addresses(cScenarioTerraformMode)
    {
        DefineAddress(SetVisualStyle, ChooseAddress(0xf33a30, 0xf339f0));
        DefineAddress(SetGroundEffectId, ChooseAddress(0xf351a0, 0xf35160));
        DefineAddress(ReloadGroundEffect, ChooseAddress(0xf31ee0, 0xf31d40));
    }

    namespace Addresses(cTerrainStateMgr)
    {
        DefineAddress(UpdateFromDefinition, ChooseAddress(0xfbc820, 0xfbc100));
    }

    namespace Addresses(cScenarioEditModeDisplayStrategy)
    {
        DefineAddress(SetMode, ChooseAddress(0xed69b0, 0xed6620));
    }

    namespace Addresses(cScenarioTerraformHistoryEntry)
    {
        DefineAddress(Undo, ChooseAddress(0xf384b0, 0xf383e0));
        DefineAddress(Redo, ChooseAddress(0xf384e0, 0xf38410));
    }

    namespace Addresses(ScenarioEditModeSculptFloraUI)
    {
        DefineAddress(UpdateFloraCategoryUI, ChooseAddress(0xee8f00, 0xee8c80));
    }

    namespace Addresses(ScenarioEditModeFileDrop)
    {
        DefineAddress(HandleMessage, ChooseAddress(0xf021d0, 0xf01e20));
    }
}
