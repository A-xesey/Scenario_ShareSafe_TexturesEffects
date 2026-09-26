#include "stdafx.h"
#include "Global.h"
#include "DebugCheats.h"
#include "VisualEffectTestCheat.h"

using namespace App;
using namespace ArgScript;
using namespace Simulator;


namespace DebugCheats
{
    VisualEffectTestCheat::VisualEffectTestCheat() { }

    VisualEffectTestCheat::~VisualEffectTestCheat() { }


    void VisualEffectTestCheat::ParseLine(const Line& line)
    {
        if (!IsScenarioMode() || line.GetArgumentsCount() - 1 < 1)
            return;
        cScenarioMode& rScenarioMode = ScenarioMode;
        if (rScenarioMode.GetMode() != cScenarioMode::Mode::EditMode)
            return;
        ResourceKey effectId = instance_id(ParseUniversal(mpFormatParser, line.GetArguments(1)[0]));
        cScenarioTerraformMode* pScenarioTerraformMode = rScenarioMode.GetTerraformMode();
        pScenarioTerraformMode->StartHistoryEntry();
        CALL(
            GetAddress(SSSTE::cScenarioTerraformMode, SetVisualStyle),
            void,
            Args(cScenarioTerraformMode*, ResourceKey&),
            Args(pScenarioTerraformMode, effectId)
        );
        pScenarioTerraformMode->CommitHistoryEntry();
    }

    const char* VisualEffectTestCheat::GetDescription(DescriptionMode mode) const
    {
        return "Replaces current visualStyle with the provided effect id.";
    }
}
