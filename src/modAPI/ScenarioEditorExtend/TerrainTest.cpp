#include "stdafx.h"
#include "TerrainTest.h"
#include <string>
#include <iostream>

using namespace Simulator;
using namespace Terrain;
using namespace App;
using namespace Editors;

TerrainTest::TerrainTest()
{
}


TerrainTest::~TerrainTest()
{
}

std::u16string uintToString2(const unsigned int& i) {
	unsigned int num = i;
	std::string s;
	while (num != 0)
	{
		int mod = num % 16;
		if (mod >= 10)
		{
			if (mod == 10) s = "A" + s;
			if (mod == 11) s = "B" + s;
			if (mod == 12) s = "C" + s;
			if (mod == 13) s = "D" + s;
			if (mod == 14) s = "E" + s;
			if (mod == 15) s = "F" + s;
		}
		else s = std::to_string(mod) + s;
		num /= 16;
	}
	while (s.length() != 8) s = "0" + s;
	return std::u16string(s.begin(), s.end());
}

auto uintFromString2(const std::u16string& i)
{
	auto hexe = std::stoul(std::string(i.begin(), i.end()), nullptr, 16);
	return hexe;
}

int i = 0;
void TerrainTest::ParseLine(const ArgScript::Line& line)
{
	/*if (PlanetModel.mpTerrain != nullptr)
	{
		cTerrainStateMgr* terrainState = PlanetModel.mpTerrain->GetTerrainStateManager();
		if (terrainState != nullptr)
		{
			terrainState->mTextures.mpTextureAboveDetail2 = TextureManager.GetRasterTexture(id("citypad_modern03"), 0);
		}
		terrainState = nullptr;
	}*/
	/*ResourceKey CustomizationID = { id("dialog-bkgrnd"), TypeIDs::png, 0x011C0C08 };
	std::u16string a = uintToString2(CustomizationID.groupID) + u"!" + uintToString2(CustomizationID.instanceID) + u"." + uintToString2(CustomizationID.typeID);
	eastl::string16 b = a.c_str();
	App::ConsolePrintF("%ls", b.c_str());
	ResourceKey CustomizationID2;

	auto groupID = b.find_first_of(u"!");
	auto typeID = b.find_last_of(u".");

	auto instanceIDl = uintFromString2(b.substr(groupID + 1, typeID - groupID - 1).c_str());
	auto lgroupID = uintFromString2(b.substr(0, groupID).c_str());
	auto ltypeID = uintFromString2(b.substr(typeID + 1, b.length() - typeID).c_str());
	App::ConsolePrintF("instanceIDl 0x%x", instanceIDl);
	App::ConsolePrintF("lgroupID 0x%x", lgroupID);
	App::ConsolePrintF("ltypeID 0x%x", ltypeID);*/
	cScenarioTerraformMode* scnTerrain = ScenarioMode.GetTerraformMode();
	if (scnTerrain != nullptr)
	{
		App::ConsolePrintF("scnTerrain exists");
		scnTerrain->StartHistoryEntry();
		/// set texture
		ResourceKey texture;
		texture.instanceID = id("CG_rock4_L2");
		//terrainThemeCliff
		scnTerrain->mpPropList->SetProperty(0x03b4f7c6, &Property().SetValueKey(texture));
		//terrainThemeAboveDetail2
		scnTerrain->mpPropList->SetProperty(0x03B4F7C9, &Property().SetValueKey(texture));
		//terrainThemeAboveDetailNoise
		scnTerrain->mpPropList->SetProperty(0x03b4f7ca, &Property().SetValueKey(texture));
		//terrainThemeBelow
		scnTerrain->mpPropList->SetProperty(0x03b4f7cb, &Property().SetValueKey(texture));
		//terrainThemeBeach2
		scnTerrain->mpPropList->SetProperty(0x03b4f7cd, &Property().SetValueKey(texture));
		//update current textures
		//cTerrainStateMgr_UpdateFromDefinition
		CALL(
			Address(ModAPI::ChooseAddress(0xf902d0, 0xfbc100)),
			void,
			Args(cTerrainStateMgr*, PropertyList*),
			Args(scnTerrain->mpTerrainStateMgr, scnTerrain->mpPropList.get()));
		/// set visual style
		//ResourceKey visEffect = { id("CR_mouselook_Master") , 0,0};
		//scnTerrain->SetVisualEffect(visEffect);
		/*CALL(
			Address(ModAPI::ChooseAddress(0xf33a30, 0xf339f0)),
			void,
			Args(cScenarioTerraformMode*, ResourceKey&),
			Args(scnTerrain, visEffect));*/
		/// set effects
		//SG_localPlanetRing_04~
		scnTerrain->mGroundEffectId = 0x03F526F8;
		//ApplyScriptSets
		CALL(
			Address(0xf34500),
			void,
			Args(cScenarioTerraformMode*),
			Args(scnTerrain));
		//// update effects
		MessageManager.MessageSend(0x36b154d8, nullptr);
		scnTerrain->mpPropList->SetProperty(0x56B14F05, &Property().SetValueInt32(scnTerrain->mGroundEffectId));
		scnTerrain->CommitHistoryEntry();
	}
	scnTerrain = nullptr;
}

const char* TerrainTest::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "This cheat does something.";
	}
	else {
		return "TerrainTest: Elaborate description of what this cheat does.";
	}
}
