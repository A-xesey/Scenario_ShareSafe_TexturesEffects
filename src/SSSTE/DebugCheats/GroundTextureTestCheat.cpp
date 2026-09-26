#include "stdafx.h"
#include "Global.h"
#include "DebugCheats.h"
#include "GroundTextureTestCheat.h"

using namespace App;
using namespace ArgScript;
using namespace Simulator;
using namespace Terrain;


namespace DebugCheats
{
    GroundTextureTestCheat::GroundTextureTestCheat() { }

    GroundTextureTestCheat::~GroundTextureTestCheat() { }


    void GroundTextureTestCheat::ParseLine(const Line& line)
    {
        if (!IsScenarioMode() || line.GetArgumentsCount() - 1 < 1)
            return;
        cScenarioMode& rScenarioMode = ScenarioMode;
        if (rScenarioMode.GetMode() != cScenarioMode::Mode::EditMode)
            return;

        ResourceKey textureKey = ParseKey(mpFormatParser, line.GetArguments(1)[0]);
        textureKey.typeID = 0x0;

        cScenarioTerraformMode* pScenarioTerraformMode = rScenarioMode.GetTerraformMode();
        pScenarioTerraformMode->StartHistoryEntry();
        PropertyList* pTerrainScript = pScenarioTerraformMode->mpPropList.get();
        pTerrainScript->SetProperty(
            PROPERTY_ID_TERRAIN_ABOVE_DETAIL2,
            &Property().SetValueKey(textureKey)
        );
        CALL(
            GetAddress(SSSTE::cTerrainStateMgr, UpdateFromDefinition),
            void,
            Args(cTerrainStateMgr*, PropertyList*),
            Args(pScenarioTerraformMode->mpTerrainStateMgr, pTerrainScript)
        );
        pScenarioTerraformMode->CommitHistoryEntry();
    }

    const char* GroundTextureTestCheat::GetDescription(DescriptionMode mode) const
    {
        return "Replaces current terrainThemeAboveDetail2 with the provided texture key.";
    }
}
