#include "GameTimeConstants.h"
#include "TownCentre.h"

#include <stdio.h>  /* For sprintf */
#include <string.h> /* For strlen */

#include <Lionhead/LH3DLib/development/LH3DObject.h>
#include <Lionhead/LHLib/ver5.0/LHWin.h> /* For operator new(size_t, const char*, uint32_t) */

#include "BuildingSite.h"
#include "Game.h"
#include "Game3DObject.h"
#include "GameOSFile.h"
#include "PlannedTownCentre.h"
#include "Player.h"
#include "Setup.h"
#include "Scaffold.h"
#include "SpellIcon.h"
#include "TotemStatue.h"
#include "SpellSeedInfo.h"
#include "Town.h"
#include "TownSpellIcon.h"
#include "WorshipSite.h"

#define TOWN_CENTRE_FILE "C:\\dev\\MP\\Black\\TownCentre.cpp"

TownCentre::TownCentre(const MapCoords& coords, const GAbodeInfo* info, Town* town, float y_angle, float scale,
                       float food, int wood)
	: Abode(coords, info, town, y_angle, scale, food, wood)
{
	SetToZero();
	GGame::g_game->GameLists.TownCentres.Add(this);
}

TownCentre::~TownCentre()
{
	if (GameObject != NULL)
	{
		GameObject->Release();
		GameObject = NULL;
	}
}

void TownCentre::SetToZero()
{
	GameObject = NULL;
	psys = NULL;
	totem_statue = NULL;
	for (int i = 0; i < MAX_TOWN_CENTRE_SPELLS; i++)
	{
		icons[i] = NULL;
	}
}

void TownCentre::ToBeDeleted(int delete_now)
{
	DeletePSys();
	DeleteDependancys();
	if (totem_statue.Get() != NULL)
	{
		totem_statue.Get()->ToBeDeleted(delete_now);
	}
	GGame::g_game->GameLists.TownCentres.Remove(this);
	Abode::ToBeDeleted(delete_now);
}

bool TownCentre::InteractsWithPhysicsObjects()
{
	return true;
}

void TownCentre::SetPlayersCreature()
{
	if (totem_statue.Get() != NULL)
	{
		totem_statue.Get()->SetPlayersCreature();
	}
}

TownCentre* TownCentre::Create(const MapCoords& coords, const GAbodeInfo* info, Town* town, float y_angle, float scale,
                               float food, int wood)
{
	TownCentre* centre = new (TOWN_CENTRE_FILE, 124) TownCentre(coords, info, town, y_angle, scale, food, wood);
	if (centre != NULL)
	{
		centre->PostCreate();
	}
	return centre;
}

void TownCentre::PostCreate()
{
	CallVirtualFunctionsForCreation(Pos);
}

bool TownCentre::CreateTotemIfNecessary()
{
	if (town.Get() != NULL && town.Get()->IsAvailable() && totem_statue.Get() == NULL && IsBuilt())
	{
		totem_statue = TotemStatue::Create(this);
	}
	return totem_statue.Get() != NULL;
}

void TownCentre::AddToPlayer()
{
	if (totem_statue.Get() != NULL)
	{
		totem_statue.Get()->AddToPlayer();
	}
}

void TownCentre::DrawAll()
{
	for (LHLinkedNode<TownCentre*>* node = GGame::g_game->GameLists.TownCentres.GetStart(); node != NULL;
	     node = node->next.Get())
	{
		if (node->payload->IsAvailable() == true)
		{
			node->payload->ProcessPSys();
			node->payload->DrawPSys();
		}
	}
}

uint32_t TownCentre::Save(GameOSFile& file)
{
	if (Abode::Save(file))
	{
		file.WritePtr(totem_statue.Get());
		file.WritePtrArray((GameThing**)icons, MAX_TOWN_CENTRE_SPELLS);
		return 1;
	}
	return 0;
}

uint32_t TownCentre::Load(GameOSFile& file)
{
	if (Abode::Load(file))
	{
		file.ReadPtr((GameThing**)&totem_statue);
		file.ReadPtrArray((GameThing**)icons);
		return 1;
	}
	return 0;
}

void TownCentre::ResolveLoad()
{
	Object::ResolveLoad();
	if (IsFunctional())
	{
		CreatePSys();
	}
}

MapCoords TownCentre::GetArrivePos()
{
	return Pos;
}

void TownCentre::StopBeingFunctional(GPlayer* player)
{
	Abode::StopBeingFunctional(player);
	if (GetTown() != NULL)
	{
		GetTown()->SetWorshipPercentage(0.0f);
	}
}

uint32_t TownCentre::GetDiscipleStateIfInteractedWith(GInterfaceStatus* status, Villager* villager)
{
	uint32_t state = Abode::GetDiscipleStateIfInteractedWith(status, villager);
	if (state == VILLAGER_DISCIPLE_MISSIONARY || state == VILLAGER_DISCIPLE_BUILDER)
	{
		return state;
	}
	if (GetTown() == villager->GetTown())
	{
		return state;
	}
	return GetTown()->IsSpaceForNewVillager() ? VILLAGER_DISCIPLE_CHANGE_HOUSE : VILLAGER_DISCIPLE_NONE;
}

float TownCentre::ReduceLife(float value, GPlayer* player)
{
	float life = Abode::ReduceLife(value, player);
	if (totem_statue.Get() != NULL)
	{
		totem_statue.Get()->SetLife(life);
	}
	return life;
}

float TownCentre::IncreaseLife(float value)
{
	float life = 0.0f;
	if (GetLife() < 1.0f)
	{
		life = Abode::IncreaseLife(value);
		if (totem_statue.Get() != NULL)
		{
			totem_statue.Get()->SetLife(life);
		}
	}
	return life;
}

void TownCentre::ReactToPhysicsImpact(PhysicsObject* physics, bool transferred_damage)
{
	Abode::ReactToPhysicsImpact(physics, transferred_damage);
}

PlannedMultiMapFixed* TownCentre::ConvertToPlanned()
{
	PlannedTownCentre* planned = new (TOWN_CENTRE_FILE, 471) PlannedTownCentre(this);
	if (planned != NULL)
	{
		planned->Init(GetTown());
		ToBeDeleted(0);
	}
	return planned;
}

PlannedTownCentre::PlannedTownCentre(TownCentre* town_centre)
	: PlannedAbode(town_centre->Pos, town_centre->GetInfo(), town_centre->GetTown(), town_centre->GetYAngle(),
                   town_centre->GetScale())
{
}

PlannedTownCentre::PlannedTownCentre(const MapCoords& coords, const GAbodeInfo* info, Town* town, float y_angle,
                                     float scale)
	: PlannedAbode(coords, info, town, y_angle, scale)
{
}

PlannedTownCentre* PlannedTownCentre::Create(const MapCoords& coords, const GAbodeInfo* info, Town* town, float y_angle,
                                             float scale)
{
	PlannedTownCentre* planned = new (TOWN_CENTRE_FILE, 505) PlannedTownCentre(coords, info, town, y_angle, scale);
	planned->Init(town);
	return planned;
}

MultiMapFixed* PlannedTownCentre::CreatePlannedNoFixedCheck(float food)
{
	float       yAngle = YAngle;
	TownCentre* centre =
		TownCentre::Create(Pos, GetInfo(), GetTown(), yAngle, GetScale(), food, TOWN_CENTRE_INITIAL_WOOD);
	if (centre != NULL)
	{
		PostCreatePlanned(*centre);
		ToBeDeleted(0);
		return centre;
	}
	return NULL;
}

int TownCentre::GetNumberOfSpells()
{
	int count = 0;
	for (int i = 0; i < MAX_TOWN_CENTRE_SPELLS; i++)
	{
		if (icons[i] != NULL)
		{
			count++;
		}
	}
	return count;
}

MapCoords TownCentre::GetTotemPos()
{
	MapCoords coords;
	coords.Clear();
	GetSpecialPos(TOWN_CENTRE_SPECIAL_POS_TOTEM, &coords);
	return coords;
}

TownCentreSpellIcon* TownCentre::FindSpellIcon(SPELL_SEED_TYPE type)
{
	for (int i = 0; i < MAX_TOWN_CENTRE_SPELLS; i++)
	{
		if (icons[i] != NULL && icons[i]->seed_info.Get() == &GSpellSeedInfo::Infos[type])
		{
			return icons[i];
		}
	}
	return NULL;
}

int TownCentre::GetSpellIconIndex(TownCentreSpellIcon* icon)
{
	for (int i = 0; i < MAX_TOWN_CENTRE_SPELLS; i++)
	{
		if (icons[i] == icon)
		{
			return i;
		}
	}
	return 0;
}

void TownCentre::AddPowerUp(SPELL_SEED_TYPE seed_type, POWER_UP_TYPE power_up_type)
{
	TownCentreSpellIcon* icon = FindSpellIcon(seed_type);
	if (icon != NULL)
	{
		icon->SetPULevel(power_up_type, true);
	}
}

void TownCentre::RemovePowerUp(SPELL_SEED_TYPE seed_type, POWER_UP_TYPE power_up_type)
{
	TownCentreSpellIcon* icon = FindSpellIcon(seed_type);
	if (icon != NULL)
	{
		icon->SetPULevel(power_up_type, false);
	}
}

void TownCentre::RemoveSpellIcon(TownCentreSpellIcon* icon)
{
	for (int i = 0; i < MAX_TOWN_CENTRE_SPELLS; i++)
	{
		if (icon == icons[i])
		{
			icons[i] = NULL;
		}
	}
}

void TownCentre::RemoveSpell(SPELL_SEED_TYPE seed_type)
{
	for (int i = 0; i < MAX_TOWN_CENTRE_SPELLS; i++)
	{
		TownCentreSpellIcon* icon = icons[i];
		if (icon != NULL && icon->seed_info.Get() == &GSpellSeedInfo::Infos[seed_type])
		{
			icons[i] = NULL;
			icon->Centre = NULL;
			if ((icon->GameThing::Flags & GAME_THING_FLAG_UNAVAILABLE) == 0)
			{
				icon->ToBeDeleted(0);
			}
		}
	}
}

void TownCentre::MakeFunctional()
{
	Abode::MakeFunctional();
	Town* town = GetTown();
	if (town != NULL)
	{
		CreateTotemIfNecessary();
		if (town->town_centre == NULL)
		{
			town->town_centre = this;
		}
		for (int i = 0; i < MAGIC_TYPE_LAST; i++)
		{
			if (town->IsMagicTypeHeld((MAGIC_TYPE)i))
			{
				SPELL_SEED_TYPE seed = GSpellSeedInfo::GetFirstSpellSeedForMagicType((MAGIC_TYPE)i);
				AddSpell(seed);
				POWER_UP_TYPE powerUp = GSpellSeedInfo::Infos[seed].GetPowerUpFromMagicType((MAGIC_TYPE)i);
				if (powerUp != POWER_UP_TYPE_NONE)
				{
					AddPowerUp(seed, powerUp);
				}
			}
		}
		WorshipSite* site = town->GetWorshipSite();
		if (site != NULL)
		{
			site->AddTownSpells(town);
		}
		CreatePSys();
	}
}

uint32_t TownCentre::SaveObject(LHOSFile& file, const MapCoords* coords)
{
	char text[200];
	char abodeText[100];
	char coordText[100];

	if ((FixedFlags & FIXED_FLAG_UNDER_CONSTRUCTION) != 0)
	{
		return Abode::SaveObject(file, coords);
	}
	uint32_t saved = CheckAndSetSaved();
	if (saved == 1)
	{
		MapCoords    relative = (coords != NULL) ? (Pos - *coords) : Pos;
		int          townId = (GetTown() != NULL) ? GetTown()->ID : TOWN_ID_NONE;
		TotemStatue* totem = totem_statue.Get();
		float        scale = GetScale();
		float        yAngle = GetYAngle();
		sprintf(text, GSetup::GetCommandAsText(SCRIPT_FEATURE_COMMANDS_CREATE_TOWN_CENTRE), townId,
		        relative.ConvertToText(coordText), GetAbodeText(abodeText),
		        (int)(yAngle * TOWN_CENTRE_SCRIPT_FIXED_POINT), (int)(scale * TOWN_CENTRE_SCRIPT_FIXED_POINT),
		        (int)(totem->WorshipSpeed * TOWN_CENTRE_SCRIPT_FIXED_POINT));
		GSetup::WriteToFile(this, file, text, strlen(text));
		return 1;
	}
	return saved;
}

bool32_t TownCentre::GetNextSpellIconPos(MapCoords& coords, float& y_angle)
{
	for (int i = 0; i < MAX_TOWN_CENTRE_SPELLS; i++)
	{
		if (icons[i] == NULL)
		{
			return Game3dObject->GetSpecialPos(i + TOWN_CENTRE_SPECIAL_POS_SPELL_ICON_FIRST, coords, y_angle) != 0;
		}
	}
	return false;
}

uint32_t TownCentre::Process()
{
	Abode::Process();
	if (totem_statue.Get() != NULL)
	{
		totem_statue.Get()->Process();
	}
	Town* town = GetTown();
#ifndef VERSION_BW1W100
	if (town != NULL && town->ZeroBaseInfluence != 0)
	{
		if (building_site == NULL || building_site->ScaffoldList.GetStart() == NULL ||
		    building_site->ScaffoldList.GetStart()->payload == NULL ||
		    building_site->ScaffoldList.GetHead()->ValidForPlaceInHand(NULL) != 1)
		{
			town->ZeroBaseInfluence = 0;
			GGame::g_game->ForceNeedUpdateInfluence();
		}
	}
#endif
	return 1;
}
