#include "GameData.h"
#include <aws/core/utils/json/JsonSerializer.h>
#include <fstream>
#include "Utils.h"
#include "Overlay.h"

using namespace Aws::Utils::Json;

UnitInfo*                         GameData::UnknownUnit = new UnitInfo();
SpellInfo*                        GameData::UnknownSpell = new SpellInfo();
std::map<std::string, UnitInfo*>  GameData::Units = {};
std::map<std::string, SpellInfo*> GameData::Spells = {};
std::map<std::string, Texture2D*> GameData::Images = {};

void GameData::Load(std::string& dataFolder)
{
	std::string unitData = dataFolder + "/UnitData.json";
	std::string spellData = dataFolder + "/SpellData.json";
	std::string spellDataCustom = dataFolder + "/SpellDataCustom.json";
	std::string spellIcons = dataFolder + "/icons_spells";
	std::string champIcons = dataFolder + "/icons_champs";

	printf("\r	Loading unit data    \n");
	LoadUnitData(unitData);

	printf("\r	Loading spell data   \n");
	LoadSpellData(spellData);
	LoadSpellData(spellDataCustom);

	printf("\r	Loading images      \n");
	LoadIcons(spellIcons);
	LoadIcons(champIcons);

	printf("\r	Loading complete                             \n");
}

UnitInfo * GameData::GetUnitInfoByName(std::string& name)
{
	auto it = Units.find(Character::ToLower(name));
	if (it != Units.end())
		return it->second;
	return UnknownUnit;
}

SpellInfo * GameData::GetSpellInfoByName(std::string& name)
{
	auto it = Spells.find(Character::ToLower(name));
	if (it != Spells.end())
		return it->second;
	return UnknownSpell;
}

void GameData::LoadUnitData(std::string&  path)
{
	std::ifstream inputUnitData;
	inputUnitData.open(path);

	if (!inputUnitData.is_open())
		throw std::runtime_error("Can't open unit data path");

	JsonValue unitData(inputUnitData);

	auto units = unitData.View().AsArray();
	for (int i = 0; i < units.GetLength(); ++i) {
		auto unitObj = units.GetItem(i).AsObject();

		UnitInfo* unit = new UnitInfo();
		unit->acquisitionRange         = unitObj.GetDouble("acquisitionRange");
		unit->attackSpeedRatio         = unitObj.GetDouble("attackSpeedRatio");
		unit->baseAttackRange          = unitObj.GetDouble("attackRange");
		unit->baseAttackSpeed          = unitObj.GetDouble("attackSpeed");
		unit->baseMovementSpeed        = unitObj.GetDouble("baseMoveSpeed");
		unit->basicAttackMissileSpeed  = unitObj.GetDouble("basicAtkMissileSpeed");
		unit->basicAttackWindup        = unitObj.GetDouble("basicAtkWindup");
		unit->gameplayRadius           = unitObj.GetDouble("gameplayRadius");
		unit->healthBarHeight          = unitObj.GetDouble("healthBarHeight");
		unit->name                     = Character::ToLower(std::string(unitObj.GetString("name").c_str()));
		unit->pathRadius               = unitObj.GetDouble("pathingRadius");
		unit->selectionRadius          = unitObj.GetDouble("selectionRadius");

		auto tags = unitObj.GetArray("tags");
		for (int j = 0; j < tags.GetLength(); ++j)
			unit->SetTag(tags.GetItem(j).AsString().c_str());

		Units[unit->name] = unit;
	}
}

void GameData::LoadSpellData(std::string& path)
{
	std::ifstream inputSpellData;
	inputSpellData.open(path);

	if (!inputSpellData.is_open())
		throw std::runtime_error("Can't open unit data path");

	JsonValue spellData(inputSpellData);

	auto spells = spellData.View().AsArray();
	for (int i = 0; i < spells.GetLength(); ++i) {
		auto spell = spells.GetItem(i).AsObject();

		SpellInfo* info = new SpellInfo();
		info->flags  = (SpellFlags)spell.GetInteger("affectsTypesFlags");
		info->delay  = spell.GetDouble("delay");
		info->height = spell.GetDouble("height");
		info->icon   = Character::ToLower(std::string(spell.GetString("icon").c_str()));
		info->name   = Character::ToLower(std::string(spell.GetString("name").c_str()));
		info->radius = spell.GetDouble("radius");
		info->range  = spell.GetDouble("range");
		info->speed  = spell.GetDouble("speed");
		info->flags  = (SpellFlags) (info->flags | (spell.GetBool("targeted") ? Targeted : 0));

		Spells[info->name] = info;
	}
}

void GameData::LoadIcons(std::string& path)
{
	std::string folder(path);
	WIN32_FIND_DATAA findData;
	HANDLE hFind;

	int nrFile = 0;
	hFind = FindFirstFileA((folder + "\\*.png").c_str(), &findData);
	do {
		if (hFind != INVALID_HANDLE_VALUE) {
			printf("\r	Loading image %d      ", nrFile++);
			std::string filePath = folder + "/" + findData.cFileName;
			Texture2D* image = Texture2D::LoadFromFile(Overlay::GetDxDevice(), filePath);
			if (image == nullptr)
				printf("Failed to load: %s\n", filePath.c_str());
			else {
				std::string fileName(findData.cFileName);
				fileName.erase(fileName.find(".png"), 4);
				Images[Character::ToLower(fileName)] = image;
			}
		}
	} while (FindNextFileA(hFind, &findData));
}

/// Some stuff we just cant get from the data files and we must hardcode it.
void GameData::SetSpellExceptions()
{
	Spells["flashfrostspell"]
		->AddFlags(FixedDestination);

	Spells["apheliosrmis"]
		->AddFlags(FixedDestination);

	Spells["bardrmissile"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(350.f);

	Spells["phosphorusbombmissile"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(250.f);

	Spells["ekkoqmis"]
		->AddFlags(FixedDestination);

	Spells["gnarqmissile"]
		->AddFlags(FixedDestination);

	Spells["gragasqmissile"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(275.f);

	Spells["gragasrboom"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(400.f);

	Spells["gravesqreturn"]
		->AddFlags(FixedDestination);

	Spells["gravessmokegrenadeboom"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(250.f);

	Spells["heimerdingerespell"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(250.f);

	Spells["heimerdingerespell_ult"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(250.f);

	Spells["jhinetrap"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(130.f);

	Spells["jinxehit"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(120.f);

	Spells["aardrmissile"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(250.f);

	Spells["luxlightstrikekugel"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(300.f);

	Spells["namiqmissile"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(180.f);

	Spells["neekoq"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(200.f);

	Spells["sejuanirmissile"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(1300.f);

	Spells["seraphineqinitialmissile"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(350.f);

	Spells["sorakaqmissile"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(235.f);

	Spells["urgotqmissile"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(180.f);

	Spells["varusemissile"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(260);

	Spells["xerathlocuspulse"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(200.f);

	Spells["ziggsqspell"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(150.f);

	Spells["ziggsw"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(240.f);

	Spells["ziggse"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(250.f);

	Spells["ziggsrboom"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(480.f);

	Spells["zileanqmissile"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(150.f);

	Spells["zoeemis"]
		->AddFlags(FixedDestination)
		->SetImpactRadius(50.f);

	Spells["viqmissile"]
		->AddFlags(FixedDestination);
}