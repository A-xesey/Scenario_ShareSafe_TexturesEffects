#include "stdafx.h"
#include "Global.h"
#include "ScenarioCustomizationPaletteCategoryIcon.h"

using namespace App;
using namespace Palettes;


member_detour(
    PaletteCategory_ReadProp,
    PaletteCategory,
    bool(const ResourceKey&, uint32_t)
)
{
    bool detoured(const ResourceKey & name, uint32_t defaultLayoutID)
    {
        if (original_function(this, name, defaultLayoutID))
        {
            PropertyListPtr pCategoryPropList;
            if (PropManager.GetPropertyList(name.instanceID, name.groupID, pCategoryPropList))
            {
                ResourceKey* pCategoryIconsModAPI;
                size_t nCategoryIconsModAPICount;
                if (Property::GetArrayKey(
                        pCategoryPropList.get(),
                        PROPERTY_ID_PALETTE_CATEGORY_ICON_LIST_MODAPI,
                        nCategoryIconsModAPICount,
                        pCategoryIconsModAPI
                    ) && nCategoryIconsModAPICount == 9
                )
                    mCategoryIconList = pCategoryIconsModAPI;
            }
            return true;
        }
        return false;
    }
};


namespace ScenarioCustomizationPaletteCategoryIcon
{
    void AttachDetours()
    {
        PaletteCategory_ReadProp::attach(GetAddress(PaletteCategory, ReadProp));
    }
}
