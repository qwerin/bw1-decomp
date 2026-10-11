#include "GameTimeConstants.h"
#include "MaxFloat.h"
#include "CreatureMimic.h"

#include "Citadel.h"
#include "ColourConstants.h" /* For White */
#include "CreatureMimicInfo.h"
#include "Game.h"
#include "InterfaceStatus.h"
#include "Object.h"
#include "Player.h"
#include "Rand.h"
#include "Town.h"
#include "Utils.h"

#define CREATURE_MIMIC_FILE "C:\\dev\\MP\\Black\\CreatureMimic.cpp"

CreatureMimicInfo CreatureMimicInfo::Infos[DETECTED_PLAYER_ACTION_LAST];

GBaseInfo* CreatureMimicInfo::GetBaseInfo(uint32_t& num_infos)
{
	num_infos = DETECTED_PLAYER_ACTION_LAST;
	return Infos;
}

CreatureMimicInfo::~CreatureMimicInfo() {}

CreatureMimicState::CreatureMimicState()
{
	Active = false;
}

CreatureMimicState::~CreatureMimicState() {}

void CreatureMimicState::SetCreatureIntoStateOfMimicking(DETECTED_PLAYER_ACTION action, GameThingWithPos* thing,
                                                         MAGIC_TYPE magic_type)
{
	Active = true;
	Completed = false;
	DetectedPlayerAction = action;
	this->magic_type = magic_type;
	game_thing = thing;
	if (thing != NULL)
	{
		coords = thing->Pos;
		ObjectMimicType = thing->GetCreatureMimicType();
	}
	Stage = 0;
	StageProgress = 0;
	uint32_t baseLimit = CreatureMimicInfo::Infos[DetectedPlayerAction].BaseStageLimit;
	StageLimit = baseLimit + GRand::GameRand(MIMIC_STAGE_LIMIT_RANDOM_RANGE, CREATURE_MIMIC_FILE, 39);
	StartTurn = GGame::g_game->data.GameTurn;
}

void Object::ConsiderCreatureMimickingWhenObjectLands()
{
	GInterfaceStatus* status = GetInterfaceStatusWhoLastDroppedMe();
	if (status != NULL)
	{
		GPlayer* player = status->GetPlayer();
		Town*    nearestTown = Pos.GetNearestTown(MaxFloat);
		Citadel* citadel = player->GetCitadel();
		Town*    town = GetTown();
		if (citadel != NULL && GUtils::GetDistanceInMetres(citadel->Pos, Pos) < MIMIC_STOLEN_OBJECT_CITADEL_RANGE)
		{
			if (town != NULL && town->GetPlayer() != player)
			{
				player->ConsiderMakingCreatureMimicPlayer(
					status, DETECTED_PLAYER_ACTION_STEAL_OBJECT_AND_PUT_BY_CITADEL, this, MAGIC_TYPE_NONE);
				return;
			}
			player->ConsiderMakingCreatureMimicPlayer(status, DETECTED_PLAYER_ACTION_TAKE_OBJECT_HOME, this,
			                                          MAGIC_TYPE_NONE);
			return;
		}
		if (town != NULL && town->GetPlayer() != player && nearestTown != NULL && nearestTown->GetPlayer() != NULL &&
		    nearestTown->GetPlayer() == player)
		{
			player->ConsiderMakingCreatureMimicPlayer(status, DETECTED_PLAYER_ACTION_STEAL_OBJECT_AND_PUT_IN_TOWN, this,
			                                          MAGIC_TYPE_NONE);
		}
	}
}
