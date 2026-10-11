#include "GameTimeConstants.h"
#include "Influence.h"

#include <stdio.h>  /* For sprintf */
#include <string.h> /* For strlen */

#include <Lionhead/LHLib/ver5.0/LHWin.h> /* For operator new(size_t, const char*, uint32_t) */

#include "BaseInfo.h"
#include "Game.h"
#include "GameOSFile.h"
#include "InfluenceInfo.h"
#include "InterfaceStatus.h"
#include "Player.h"
#include "Setup.h"
#include "Utils.h"
#include "VirtualInfluence.h"

#define FILEPATH "C:\\dev\\MP\\Black\\Influence.h"

float Influence::CalculatePlayerInfluence(const MapCoords& pos, GPlayer* player, int calc_flags, INFL_CALC_TYPE type,
                                          int include_allies)
{
	if (player == NULL)
	{
		return 0.0f;
	}
	if ((GGame::g_game->GameFlags & GAME_FLAG_ACTIVATE_CREATURE_DESIRES) == 0 && player->InfiniteInfluence == 0)
	{
		GInterfaceStatus* status = player->GetNextInterfaceStatus(NULL);
		while (status != NULL)
		{
			float influence = status->influence->GetInterfaceStatusInfluence(pos, type);
			if (influence != 0.0f)
			{
				return influence;
			}
			status = player->GetNextInterfaceStatus(status);
		}
		float influence = CalculatePlayerRawInfluence(pos, player, calc_flags);
		if (include_allies != 0 && influence <= 0.0f)
		{
			return CalculateAllyInfluence(pos, player);
		}
		return influence;
	}
	return 1.0f;
}

float Influence::CalculateAllyInfluence(const MapCoords& pos, GPlayer* player)
{
	GPlayer* ally = player->GetNextAllyPlayer(NULL);
	while (ally != NULL)
	{
		if (player->CanAllyUseInfluence(ally))
		{
			float influence = CalculatePlayerRawInfluence(pos, ally, 0);
			if (influence > 0.0f)
			{
				return influence;
			}
		}
		ally = player->GetNextAllyPlayer(ally);
	}
	return 0.0f;
}

bool32_t Influence::IsInPlayerRawInfluence(const MapCoords& pos, GPlayer* player)
{
	return CalculatePlayerRawInfluence(pos, player, 0) > 0.0f;
}

bool32_t Influence::IsInAntiInfluence(const MapCoords& pos, GPlayer* player)
{
	FOREACH_LH_LIST_HEAD_SAFE(InfluenceRing, ring, GGame::g_game->GameLists.InfluenceRingList)
	{
		if (ring->GetPlayer() == player && ring->Anti != 0 && ring->IsInInfluence(pos))
		{
			return true;
		}
	}
	return false;
}

float Influence::CalculateInfluenceOnRange(float distance, float radius, int)
{
	float inner = GInfluenceInfo::Info.FullInfluenceFraction * radius;
	if (distance <= inner)
	{
		return 1.0f;
	}
	float outer = (GInfluenceInfo::Info.FalloffFraction + GInfluenceInfo::Info.FullInfluenceFraction) * radius;
	if (distance <= outer)
	{
		return (1.0f - (distance - inner) / (outer - inner)) * (1.0f - GInfluenceInfo::Info.FalloffLoss);
	}
	if (distance < radius)
	{
		return (1.0f - (distance - outer) / (radius - outer)) * OUTER_RING_INFLUENCE_FRACTION;
	}
	return 0.0f;
}

GPlayer* Influence::CalculateMostInfluentialPlayer(const MapCoords& pos, float* influence)
{
	float    bestInfluence = 0.0f;
	GPlayer* bestPlayer = &GGame::g_game->players[GGame::g_game->NeutralPlayerIndex];
	GPlayer* player = GGame::g_game->GetNextPlayer(NULL);
	while (player != NULL)
	{
		float playerInfluence = CalculatePlayerInfluence(pos, player, 0, INFL_CALC_TYPE_DEFAULT, 1);
		if (playerInfluence > bestInfluence)
		{
			bestInfluence = playerInfluence;
			bestPlayer = player;
		}
		player = GGame::g_game->GetNextPlayer(player);
	}
	if (influence != NULL)
	{
		*influence = bestInfluence;
	}
	return bestPlayer;
}

InfluenceRing::InfluenceRing(const MapCoords& coords, GPlayer* player, float radius, int anti)
{
	SetThing(NULL);
	Pos = coords;
	this->player = player;
	Influence = radius;
	GGame::g_game->GameLists.InfluenceRingList.AddToFirst(this);
	Anti = anti;
}

InfluenceRing::InfluenceRing(GameThingWithPos* thing, GPlayer* player, float radius, int anti)
{
	SetThing(thing);
	Pos = thing->Pos;
	this->player = player;
	Influence = radius;
	GGame::g_game->GameLists.InfluenceRingList.AddToFirst(this);
	Anti = anti;
}

void InfluenceRing::ToBeDeleted(int delete_now)
{
	GGame::g_game->GameLists.InfluenceRingList.Remove(this);
	GameThingWithPos::ToBeDeleted(delete_now);
}

float InfluenceRing::CalculateInfluence(const MapCoords& pos, GPlayer* player, int calc_flags)
{
	if (player != GetPlayer())
	{
		return 0.0f;
	}
	if (Anti != 0)
	{
		return 0.0f;
	}
	float distance = GUtils::GetDistanceInMetres(Pos, pos);
	return ::Influence::CalculateInfluenceOnRange(distance, Influence, calc_flags);
}

void InfluenceRing::SetThing(GameThingWithPos* thing)
{
	if (thing != NULL)
	{
		info.Set(thing);
	}
	else
	{
		info.AddressOffset = 0;
		info.UniqueId = 0;
	}
}

InfluenceRing* InfluenceRing::Create(GameThingWithPos* thing, GPlayer* player, float radius, int anti)
{
	return new (FILEPATH, 400) InfluenceRing(thing, player, radius, anti);
}

InfluenceRing* InfluenceRing::Create(const MapCoords& coords, GPlayer* player, float radius, int anti)
{
	return new (FILEPATH, 410) InfluenceRing(coords, player, radius, anti);
}

bool32_t InfluenceRing::IsInInfluence(const MapCoords& pos) const
{
	return GUtils::GetDistanceInMetres(Pos, pos) <= Influence;
}

uint32_t InfluenceRing::SaveObject(LHOSFile& file, const MapCoords* coords)
{
	char      text[200];
	char      coordText[100];
	MapCoords relative;

	if (coords != NULL)
	{
		relative = Pos - *coords;
	}
	if (info.IsClear())
	{
		sprintf(text, GSetup::GetCommandAsText(SCRIPT_FEATURE_COMMANDS_CREATE_INFLUENCE_RING),
		        Pos.ConvertToText(coordText), GetPlayer()->GetPlayerNumber(), Influence);
		GSetup::WriteToFile(this, file, text, strlen(text));
		return 1;
	}
	return 0;
}

void InfluenceRing::UpdateInfluence(float radius)
{
	Influence = radius;
}

void InfluenceRing::ProcessRings()
{
	InfluenceRing* ring = GGame::g_game->GameLists.InfluenceRingList.Get();
	while (ring != NULL)
	{
		InfluenceRing* next = GGame::g_game->GameLists.InfluenceRingList.GetNext(ring);
		ring->Process();
		ring = next;
	}
}

GameThingWithPos* InfluenceRing::GetThing()
{
	if (!info.IsClear())
	{
		if (info.IsValid())
		{
			return (GameThingWithPos*)info.GetBase();
		}
	}
	return NULL;
}

void InfluenceRing::Process()
{
	if (!info.IsClear())
	{
		if (info.IsValid())
		{
			Pos = ((GameThingWithPos*)info.GetBase())->Pos;
		}
		else
		{
			ToBeDeleted(0);
		}
	}
}

SCRIPT_OBJECT_TYPE InfluenceRing::GetScriptObjectType()
{
	return SCRIPT_OBJECT_TYPE_INFLUENCE_RING;
}

uint32_t InfluenceRing::Save(GameOSFile& file)
{
	if (GameThingWithPos::Save(file))
	{
		file.WritePtr(dynamic_cast<GameThingWithPos*>(info.GetBase()));
		file.WritePtr(player.Get());
		file.WriteSafe((uint32_t&)Influence);
		file.WriteSafe((uint32_t&)Anti);
		return 1;
	}
	return 0;
}

uint32_t InfluenceRing::Load(GameOSFile& file)
{
	if (GameThingWithPos::Load(file))
	{
		GameThing* thing;
		file.ReadPtr(&thing);
		info.Set(thing);
		file.ReadPtr((GameThing**)&player);
		file.ReadSafe((uint32_t&)Influence);
		file.ReadSafe((uint32_t&)Anti);
		return 1;
	}
	return 0;
}

GBaseInfo* GInfluenceInfo::GetBaseInfo(uint32_t& num_infos)
{
	num_infos = 0;
	return NULL;
}

GInfluenceInfo::~GInfluenceInfo() {}
