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

#define MIMIC_STOLEN_OBJECT_CITADEL_RANGE 80.0f
#define MIMIC_STAGE_LIMIT_RANDOM_RANGE    1

CreatureMimicInfo CreatureMimicInfo::Infos[DETECTED_PLAYER_ACTION_LAST];

GBaseInfo* CreatureMimicInfo::GetBaseInfo(uint32_t& param_1)
{
	param_1 = DETECTED_PLAYER_ACTION_LAST;
	return Infos;
}

CreatureMimicInfo::~CreatureMimicInfo() {}

CreatureMimicState::CreatureMimicState()
{
	field_0x8 = 0;
}

CreatureMimicState::~CreatureMimicState() {}

void CreatureMimicState::SetCreatureIntoStateOfMimicking(DETECTED_PLAYER_ACTION action, GameThingWithPos* thing,
                                                         MAGIC_TYPE magic_type)
{
	field_0x8 = 1;
	field_0xc = 0;
	DetectedPlayerAction = action;
	this->magic_type = magic_type;
	game_thing = thing;
	if (thing != NULL)
	{
		coords = thing->Pos;
		field_0x1c = thing->GetCreatureMimicType();
	}
	Stage = 0;
	StageProgress = 0;
	uint32_t baseLimit = CreatureMimicInfo::Infos[DetectedPlayerAction].BaseStageLimit;
	StageLimit = baseLimit + GRand::GameRand(MIMIC_STAGE_LIMIT_RANDOM_RANGE, CREATURE_MIMIC_FILE, 39);
	field_0x2c = GGame::g_game->data.GameTurn;
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
