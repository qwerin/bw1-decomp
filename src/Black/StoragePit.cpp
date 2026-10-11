#include "GameTimeConstants.h"
#include "StoragePit.h"

#include "AbodeInfo.h"
#include "Game.h"
#include "Game3DObject.h"
#include "GameOSFile.h"
#include "HelpProfile.h"
#include "InterfaceStatus.h"
#include "PhysicsObject.h"
#include "Player.h"
#include "Rand.h"
#include "Reaction.h"
#include "PileFood.h"
#include "PileWood.h"
#include "Pot.h"
#include "PotStructure.h"
#include "PotInfo.h"
#include "Town.h"
#include "TownDesireFlags.h"

#define FILEPATH "C:\\dev\\MP\\Black\\StoragePit.cpp"

StoragePit::StoragePit(const MapCoords& coords, const GAbodeInfo* info, Town* town, float y_angle, float scale,
                       float food, int wood)
	: Abode(coords, info, town, y_angle, scale, food, wood)
{
	FoodPiles[0] = NULL;
	for (int i = 0; i < MAX_WOOD_PILES; i++)
	{
		WoodPiles[i] = NULL;
	}
}

StoragePit::~StoragePit() {}

void StoragePit::Delete()
{
	DeleteDependancys();
	Abode::Delete();
}

void StoragePit::ToBeDeleted(int delete_now)
{
	DeleteDependancys();
	if (FoodPiles[0] != NULL && FoodPiles[0]->IsAvailable())
	{
		FoodPiles[0]->SetMultiMapFixed(NULL);
		FoodPiles[0]->ToBeDeleted(delete_now);
		FoodPiles[0] = NULL;
	}
	for (int i = 0; i < MAX_WOOD_PILES; i++)
	{
		if (WoodPiles[i] != NULL && WoodPiles[i]->IsAvailable())
		{
			WoodPiles[i]->SetMultiMapFixed(NULL);
			WoodPiles[i]->ToBeDeleted(delete_now);
			WoodPiles[i] = NULL;
		}
	}
	Abode::ToBeDeleted(delete_now);
}

void StoragePit::DeleteDependancys()
{
	if (GetTown() != NULL && GetTown()->GetStoragePit() == this)
	{
		LHListHead<Abode>* list = &GetTown()->AbodeList;
		Abode*             abode;
		for (abode = list->head; abode != NULL; abode = abode->next)
		{
			if ((abode->GetInfo()->AbodeType & ABODE_TYPE_STORAGE_PIT) && abode->IsFunctional() && this != abode)
			{
				break;
			}
		}
		GetTown()->SetStoragePit(dynamic_cast<StoragePit*>(abode));
	}
	Abode::DeleteDependancys();
}

StoragePit* StoragePit::Create(const MapCoords& coords, const GAbodeInfo* info, Town* town, float y_angle, float scale,
                               float food, int wood)
{
	StoragePit* pit = new (FILEPATH, 110) StoragePit(coords, info, town, y_angle, scale, food, wood);
	if (pit != NULL)
	{
		pit->CallVirtualFunctionsForCreation(coords);
		pit->FoodPiles[0] = (PileFood*)Pot::Create(pit->GetResourcePos(RESOURCE_TYPE_FOOD, 0),
		                                           &GPotInfo::InfoList[POT_INFO_STORAGE_PIT_FOOD_PILE], 0, pit, NULL, 0,
		                                           pit->GetYAngle(), 1.0f, 0);
		for (uint32_t i = 0; i < MAX_WOOD_PILES; i++)
		{
			pit->WoodPiles[i] = (PileWood*)Pot::Create(pit->GetResourcePos(RESOURCE_TYPE_WOOD, i),
			                                           &GPotInfo::InfoList[POT_INFO_WOOD_PILE_1 + i], 0, pit, NULL, 0,
			                                           pit->GetYAngle(), 1.0f, 0);
		}
		pit->InitDesireFlags();
	}
	return pit;
}

void StoragePit::CallVirtualFunctionsForCreation(const MapCoords& coords)
{
	Abode::CallVirtualFunctionsForCreation(Pos);
	Game3dObject->UpdateMelting();
}

void StoragePit::MakeFunctional()
{
	Abode::MakeFunctional();
	if (GetTown() != NULL)
	{
		GetTown()->SetStoragePit(this);
	}
}

void StoragePit::InitDesireFlags()
{
	MapCoords flagPos;
	Town*     town = GetTown();
	if (town != NULL)
	{
		for (int i = 0; i < TOWN_DESIRE_INFO_LAST; i++)
		{
			if (town->town_desire_flags[i] != NULL)
			{
				flagPos = GetTownDesireFlagPos(town->town_desire_flags[i]->GetSlot());
				town->town_desire_flags[i]->ActualMoveMapObject(flagPos);
			}
		}
	}
}

MapCoords StoragePit::GetResourcePos(RESOURCE_TYPE type, unsigned long index)
{
	MapCoords pos;
	if (type == RESOURCE_TYPE_FOOD)
	{
		GetSpecialPos(index + STORAGE_PIT_SPECIAL_POS_FOOD_PILE, &pos);
	}
	else if (type == RESOURCE_TYPE_WOOD)
	{
		GetSpecialPos(index + STORAGE_PIT_SPECIAL_POS_WOOD_PILE, &pos);
	}
	else
	{
		return Pos;
	}
	return pos;
}

MapCoords StoragePit::GetTownDesireFlagPos(unsigned long index)
{
	MapCoords pos;
	GetSpecialPos(index + STORAGE_PIT_SPECIAL_POS_TOWN_DESIRE_FLAG, &pos);
	return pos;
}

PileResource* StoragePit::GetResourcePile(RESOURCE_TYPE type, unsigned long index)
{
	PileResource* pile;
	if (type == RESOURCE_TYPE_FOOD)
	{
		pile = FoodPiles[index];
	}
	else
	{
		pile = WoodPiles[index];
	}
	if (pile != NULL && !pile->IsAvailable())
	{
		return NULL;
	}
	return pile;
}

bool StoragePit::IsPoisonedResource(RESOURCE_TYPE type)
{
	if (type == RESOURCE_TYPE_FOOD)
	{
		for (uint32_t i = 0; i < MAX_FOOD_PILES; i++)
		{
			if (FoodPiles[i] != NULL && FoodPiles[i]->IsAvailable() && FoodPiles[i]->IsPoisoned())
			{
				return true;
			}
		}
	}
	else if (type == RESOURCE_TYPE_WOOD)
	{
		for (uint32_t i = 0; i < MAX_WOOD_PILES; i++)
		{
			if (WoodPiles[i] != NULL && WoodPiles[i]->IsAvailable() && WoodPiles[i]->IsPoisoned())
			{
				return true;
			}
		}
	}
	return false;
}

bool StoragePit::IsPoisoned()
{
	return IsPoisonedResource(RESOURCE_TYPE_FOOD) || IsPoisonedResource(RESOURCE_TYPE_WOOD);
}

void StoragePit::SetPoisoned(int poisoned)
{
	SetPoisonedResource(RESOURCE_TYPE_FOOD, poisoned);
}

void StoragePit::SetPoisonedResource(RESOURCE_TYPE type, int poisoned)
{
	if (type == RESOURCE_TYPE_FOOD)
	{
		if (FoodPiles[0] != NULL && FoodPiles[0]->IsAvailable())
		{
			FoodPiles[0]->SetPoisonedResource(RESOURCE_TYPE_FOOD, poisoned || FoodPiles[0]->IsPoisoned());
		}
	}
	else if (type == RESOURCE_TYPE_WOOD)
	{
		for (int i = 0; i < MAX_WOOD_PILES; i++)
		{
			if (WoodPiles[i] != NULL && WoodPiles[i]->IsAvailable())
			{
				WoodPiles[i]->SetPoisonedResource(RESOURCE_TYPE_WOOD, poisoned || WoodPiles[i]->IsPoisoned());
			}
		}
	}
}

void StoragePit::ReactToPhysicsImpact(PhysicsObject* physics, bool transferred_damage)
{
	Abode::ReactToPhysicsImpact(physics, transferred_damage);
}

uint32_t StoragePit::GetDiscipleStateIfInteractedWith(GInterfaceStatus* status, Villager* villager)
{
	uint32_t state = Abode::GetDiscipleStateIfInteractedWith(status, villager);
	if (state == VILLAGER_DISCIPLE_MISSIONARY)
	{
		state = VILLAGER_DISCIPLE_TRADER;
	}
	return state;
}

uint32_t StoragePit::Save(GameOSFile& file)
{
	if (Abode::Save(file))
	{
		file.WritePtrArray((GameThing**)FoodPiles, MAX_FOOD_PILES);
		file.WritePtrArray((GameThing**)WoodPiles, MAX_WOOD_PILES);
		return true;
	}
	return false;
}

uint32_t StoragePit::Load(GameOSFile& file)
{
	if (Abode::Load(file))
	{
		file.ReadPtrArray((GameThing**)FoodPiles);
		file.ReadPtrArray((GameThing**)WoodPiles);
		return true;
	}
	return false;
}

void StoragePit::StopBeingFunctional(GPlayer* player)
{
	Abode::StopBeingFunctional(player);
	if (FoodPiles[0] != NULL && FoodPiles[0]->JustGetResource(RESOURCE_TYPE_FOOD, 0, NULL))
	{
		FoodPiles[0]->SetupReaction();
	}
	for (int i = 0; i < MAX_WOOD_PILES; i++)
	{
		if (WoodPiles[i] != NULL && WoodPiles[i]->JustGetResource(RESOURCE_TYPE_WOOD, 0, NULL))
		{
			WoodPiles[i]->SetupReaction();
		}
	}
}

void StoragePit::RestartBeingFunctional()
{
	if (FoodPiles[0] != NULL && FoodPiles[0]->IsAvailable())
	{
		FoodPiles[0]->RemoveReaction();
	}
	for (int i = 0; i < MAX_WOOD_PILES; i++)
	{
		if (WoodPiles[i] != NULL && WoodPiles[i]->IsAvailable())
		{
			WoodPiles[i]->RemoveReaction();
		}
	}
}

bool32_t StoragePit::DeleteObjectAndTakeResource(Object* object, GInterfaceStatus* status)
{
	GPlayer* player = NULL;
	if (status != NULL)
	{
		player = status->GetPlayer();
	}
	if (object->Flags & GAME_THING_WITH_POS_FLAG_IN_PHYSICS)
	{
		PhysicsObject* physics = PhysicsObject::SearchForPhysicsObject(object);
		if (physics != NULL)
		{
			GInterfaceStatus* physicsStatus = physics->status;
			if (physicsStatus == GGame::g_game->MyInterfaceStatus())
			{
				GGame::g_game->help_profile->Trigger(HELP_EVENT_TYPE_6);
			}
		}
	}
	DoDeleteObjectAndTakeResource(object, status);
	Reaction::CreateReaction(this, REACTION_REACT_TO_HAND_PUTTING_STUFF_IN_STORAGE_PIT, player, 1);
	return true;
}

bool32_t StoragePit::DoCreatureMimicAfterAddingResource(RESOURCE_TYPE type, GInterfaceStatus& status)
{
	if (!Abode::DoCreatureMimicAfterAddingResource(type, status))
	{
		if (type == RESOURCE_TYPE_FOOD)
		{
			GPlayer* owner = (GPlayer*)status.field_0x128;
			if (owner != status.GetPlayer())
			{
				status.GetPlayer()->ConsiderMakingCreatureMimicPlayer(
					&status,
					GRand::GameRand(2, FILEPATH, 688) ? DETECTED_PLAYER_ACTION_STEAL_FOOD_FROM_STORAGE_PIT
													  : DETECTED_PLAYER_ACTION_STEAL_FOOD_FROM_FARM,
					this, MAGIC_TYPE_NONE);
			}
			else
			{
				status.GetPlayer()->ConsiderMakingCreatureMimicPlayer(
					&status, DETECTED_PLAYER_ACTION_PUT_FOOD_IN_STORAGE_PIT, this, MAGIC_TYPE_NONE);
			}
		}
		else
		{
			GPlayer* owner = (GPlayer*)status.field_0x128;
			if (owner != status.GetPlayer())
			{
				status.GetPlayer()->ConsiderMakingCreatureMimicPlayer(
					&status, DETECTED_PLAYER_ACTION_STEAL_WOOD_FROM_STORAGE_PIT, this, MAGIC_TYPE_NONE);
			}
			else
			{
				status.GetPlayer()->ConsiderMakingCreatureMimicPlayer(
					&status, DETECTED_PLAYER_ACTION_PUT_WOOD_IN_STORAGE_PIT, this, MAGIC_TYPE_NONE);
			}
		}
	}
	return true;
}

uint32_t StoragePit::AddResource(RESOURCE_TYPE type, uint32_t amount, GInterfaceStatus* status, bool poisoned,
                                 const MapCoords* coords, int from_other_player)
{
	if (building_site != NULL && (type == RESOURCE_TYPE_WOOD || type == RESOURCE_TYPE_ANY))
	{
		return building_site->AddResource(type, amount, status, poisoned, NULL, 0);
	}
	uint32_t added = 0;
	if (type == RESOURCE_TYPE_FOOD)
	{
		for (uint32_t i = 0; i < MAX_FOOD_PILES; i++)
		{
			if (amount == 0)
			{
				break;
			}
			if (FoodPiles[i] == NULL)
			{
				FoodPiles[i] = (PileFood*)Pot::Create(GetResourcePos(RESOURCE_TYPE_FOOD, i),
				                                      &GPotInfo::InfoList[POT_INFO_STORAGE_PIT_FOOD_PILE + i], 0, this,
				                                      NULL, 0, GetYAngle(), 1.0f, 0);
				FoodPiles[i]->SetPoisoned(poisoned || FoodPiles[i]->IsPoisoned());
			}
			if (FoodPiles[i] != NULL && FoodPiles[i]->IsAvailable())
			{
				uint32_t result = FoodPiles[i]->JustAddResource(RESOURCE_TYPE_FOOD, amount, poisoned);
				added += result;
				amount -= result;
			}
		}
	}
	else if (type == RESOURCE_TYPE_WOOD)
	{
		for (uint32_t i = 0; i < MAX_WOOD_PILES; i++)
		{
			if (amount == 0)
			{
				break;
			}
			if (WoodPiles[i] == NULL)
			{
				WoodPiles[i] = (PileWood*)Pot::Create(GetResourcePos(RESOURCE_TYPE_WOOD, i),
				                                      &GPotInfo::InfoList[POT_INFO_WOOD_PILE_1 + i], 0, this, NULL, 0,
				                                      GetYAngle(), 1.0f, 0);
				WoodPiles[i]->SetPoisoned(poisoned || WoodPiles[i]->IsPoisoned());
			}
			if (WoodPiles[i] != NULL && WoodPiles[i]->IsAvailable())
			{
				uint32_t result = WoodPiles[i]->JustAddResource(RESOURCE_TYPE_WOOD, amount, poisoned);
				added += result;
				amount -= result;
			}
		}
	}
	if (GetResource(type) == 0 && added != 0 && GetTown() != NULL)
	{
		Town* town = GetTown();
		town->DesireDirty = true;
		town->DesireDirtyTimer = 0;
	}
	DoResourceAdding(type, added, status, poisoned, *coords, from_other_player);
	return added;
}

uint32_t StoragePit::RemoveResource(RESOURCE_TYPE type, uint32_t amount, GInterfaceStatus* status, bool* poisoned)
{
	uint32_t removed = 0;
	if (type == RESOURCE_TYPE_FOOD)
	{
		for (int i = MAX_FOOD_PILES - 1; i >= 0; i--)
		{
			if (amount == 0)
			{
				break;
			}
			if (FoodPiles[i] != NULL)
			{
				uint32_t result = FoodPiles[i]->JustRemoveResource(RESOURCE_TYPE_FOOD, amount, NULL);
				removed += result;
				amount -= result;
			}
		}
	}
	else if (type == RESOURCE_TYPE_WOOD)
	{
		for (int i = MAX_WOOD_PILES - 1; i >= 0; i--)
		{
			if (amount == 0)
			{
				break;
			}
			if (WoodPiles[i] != NULL)
			{
				uint32_t result = WoodPiles[i]->JustRemoveResource(RESOURCE_TYPE_WOOD, amount, NULL);
				removed += result;
				amount -= result;
			}
		}
	}
	if (removed != 0)
	{
		DoResourceRemoving(type, removed, status, poisoned);
	}
	return removed;
}
