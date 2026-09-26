#pragma once

extern string16 g_sTextureDefinitionsImportPath;
extern string16 g_sEffectDefinitionsImportPath;
extern bool g_bDefinitionPathsInitialized;
extern uint32_t g_nSaveGroupId;

// Model-in-Picture import functionality for texture/effect definitions and everything about it.
namespace ScenarioCustomizationItemDefinitionImport
{
    void AttachDetours();
};
