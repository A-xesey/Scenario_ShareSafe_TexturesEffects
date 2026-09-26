#pragma once

extern string16 g_sTextureDefinitionsImportPath;
extern string16 g_sEffectDefinitionsImportPath;
extern bool g_bDefinitionPathsInitialized;
extern uint32_t g_nSaveGroupId;

namespace ScenarioCustomizationItemDefinitionImport
{
    void AttachDetours();
};
