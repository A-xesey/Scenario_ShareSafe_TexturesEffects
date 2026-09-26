#include "stdafx.h"
#include "Global.h"
#include "ScenarioCustomizationItemDefinitionImport.h"

using namespace App;

string16 g_sTextureDefinitionsImportPath;
string16 g_sEffectDefinitionsImportPath;
bool g_bDefinitionPathsInitialized = false;
uint32_t g_nSaveGroupId = 0x0;


member_detour(
    Thumbnail_cImportExport_ImportPNG,
    Thumbnail_cImportExport,
    bool(const char16_t*, ResourceKey&)
)
{
    bool detoured(const char16_t* path, ResourceKey& key)
    {
        bool bResult = original_function(this, path, key);
        if (bResult)
            g_nSaveGroupId = key.groupID;
        return bResult;
    }
};

member_detour(
    Thumbnail_cImportExport_GetFolderPath,
    Thumbnail_cImportExport,
    bool(uint32_t, string16&)
)
{
    bool detoured(uint32_t creationType, string16& dst)
    {
        if (creationType == TypeIDs::prop && g_bDefinitionPathsInitialized)
        {
            const uint32_t nSaveGroupId = g_nSaveGroupId;
            g_nSaveGroupId = 0x0;
            switch (nSaveGroupId)
            {
            case GROUP_ID_TEXTURES_DEFINITIONS:
                dst = g_sTextureDefinitionsImportPath;
                return true;
            case GROUP_ID_EFFECTS_DEFINITIONS:
                dst = g_sEffectDefinitionsImportPath;
                return true;
            }
        }
        return original_function(this, creationType, dst);
    }
};

member_detour(
    Thumbnail_cImportExport_ImportDirectoryPNGs,
    Thumbnail_cImportExport,
    bool(const string16&, hash_set<string16>&, int&)
)
{
    bool detoured(const string16& directoryPath, hash_set<string16>& dstSkippedPaths, int& dstCount)
    {
        bool bResult = original_function(this, directoryPath, dstSkippedPaths, dstCount);
        if (directoryPath == mScenariosPath)
        {
            if (!g_bDefinitionPathsInitialized)
            {
                FolderPathFromLocale(
                    GROUP_ID_TEXTURES_DEFINITIONS,
                    g_sTextureDefinitionsImportPath,
                    TABLE_ID_SAVE_AREAS
                );
                FolderPathFromLocale(
                    GROUP_ID_EFFECTS_DEFINITIONS,
                    g_sEffectDefinitionsImportPath,
                    TABLE_ID_SAVE_AREAS
                );
                g_bDefinitionPathsInitialized = true;
            }
            original_function(this, g_sTextureDefinitionsImportPath, dstSkippedPaths, dstCount);
            original_function(this, g_sEffectDefinitionsImportPath, dstSkippedPaths, dstCount);
        }
        return bResult;
    }
};


namespace ScenarioCustomizationItemDefinitionImport
{
    void AttachDetours()
    {
        Thumbnail_cImportExport_ImportPNG::attach(GetAddress(Thumbnail_cImportExport, ImportPNG));
        Thumbnail_cImportExport_GetFolderPath::attach(
            GetAddress(Thumbnail_cImportExport, GetFolderPath)
        );
        Thumbnail_cImportExport_ImportDirectoryPNGs::attach(
            GetAddress(Thumbnail_cImportExport, ImportDirectoryPNGs)
        );
    }
}
