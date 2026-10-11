#include "GameTimeConstants.h"
#include "Belief.h"

#include "BeliefInfo.h"
#include "ColourConstants.h" /* For White */
#include "Game.h"
#include "Player.h"
#include "Reaction.h"
#include "Town.h"
#include "Utils.h"
#include "ValueSpinner.h"

GBeliefInfo GBeliefInfo::Info;

GBaseInfo* GBeliefInfo::GetBaseInfo(uint32_t& param_1)
{
	param_1 = 1;
	return &Info;
}

void GBelief::Init(Town* town)
{
	for (int i = 0; i < _PLAYER_NAME_COUNT; i++)
	{
		BeliefInPlayer[i] = 0.0f;
		BeliefInPlayerMax[i] = 10.0f;
		field_0xc8[i] = 0.0f;
		field_0xa8[i] = 0.0f;
		field_0x48[i] = 0;
	}
	GPlayer* neutral = &GGame::g_game->players[GGame::g_game->NeutralPlayerIndex];
	float    neutralBelief = town->GetBeliefInNeutralPlayer();
	BeliefInPlayer[neutral->GetPlayerNumber()] = neutralBelief;
	for (int j = 0; j < NUM_REACTION_FUNCTIONS; j++)
	{
		BoredomMultiplier[j] = 1.0f;
	}
	fn_00437E50();
}

float GBelief::GetBeliefInPlayer(unsigned long param_1)
{
	return BeliefInPlayer[param_1];
}

float GBelief::GetBeliefInPlayer(GPlayer* player)
{
	return BeliefInPlayer[player->GetPlayerNumber()];
}

void GBelief::AddToBelief(GPlayer* player, float amount, GameThingWithPos* object, int param_4,
                          GUIDANCE_ALIGNMENT alignment)
{
	unsigned long playerNumber = player->GetPlayerNumber();
	field_0xc8[playerNumber] += amount;
	field_0x28[playerNumber] += amount;
	if (amount != 0.0f)
	{
		field_0x48[playerNumber] = GGame::g_game->data.GameTurn;
	}
	if (object != NULL)
	{
		if (param_4 != 0)
		{
			DrawBelief(amount, object, player);
		}
		fn_00437F40(player, &object->Pos, alignment);
	}
}

void GBelief::ReduceBelief(GPlayer* player, Town* town, float amount)
{
	unsigned long playerNumber = player->GetPlayerNumber();
	BeliefInPlayer[playerNumber] -= amount;
	if (town->town_centre != NULL)
	{
		field_0xa8[playerNumber] += amount;
		if (field_0xa8[playerNumber] > 0.005)
		{
			GameThingWithPos* centre = town->town_centre;
			DrawBelief(-field_0xa8[playerNumber], centre, GGame::g_game->GetPlayer(playerNumber));
			field_0xa8[playerNumber] = 0.0f;
		}
	}
}

float GBelief::fn_00438060(unsigned long player_number)
{
	return field_0x88[player_number];
}

float GBelief::DistanceChangeToBelief(float param_1, float param_2)
{
	return GUtils::SigmoidThreshold(-0.9f, -(param_1 / param_2));
}

void GBelief::AddToBoredomMultiplier(Reaction* reaction, float amount)
{
	REACTION type = reaction->type;
	float    value = amount + BoredomMultiplier[type];
	if (value >= 0.0f)
	{
		BoredomMultiplier[type] = value;
	}
	else
	{
		BoredomMultiplier[type] = 0.0f;
	}
}

void GBelief::SetBelief(unsigned long index, float value)
{
	BeliefInPlayer[index] = BeliefInPlayerMax[index] < value ? BeliefInPlayerMax[index] : value;
}

float GBelief::GetBeliefNeededToConvert(GPlayer* player, Town* town)
{
	if (town->GetPlayer() == player)
	{
		float best = -9999.0f;
		float result = 0.0f;
		for (unsigned long i = 0; i < _PLAYER_NAME_COUNT; i++)
		{
			if (player->GetPlayerNumber() != i)
			{
				float difference = GetBeliefInPlayer(i) - GetBeliefInPlayer(player);
				if (difference > best)
				{
					best = difference;
					result = GetBeliefInPlayer(i);
				}
			}
		}
		return result;
	}
	return GetBeliefInPlayer(town->GetPlayer());
}

float GBelief::GetMaxBeliefMeNotIncluded(unsigned long player_number)
{
	float max = 0.0f;
	for (unsigned long i = 0; i < _PLAYER_NAME_COUNT; i++)
	{
		if (player_number != i && GetBeliefInPlayer(i) > max)
		{
			max = GetBeliefInPlayer(i);
		}
	}
	return max;
}

void GBelief::SetBeliefInPlayerCap(GPlayer* player, float cap)
{
	BeliefInPlayerMax[player->GetPlayerNumber()] = cap;
}

float GBelief::GetBeliefInPlayerCap(GPlayer* player)
{
	return BeliefInPlayerMax[player->GetPlayerNumber()];
}

float GBelief::GetPercentCloseOtherPlayer(GPlayer* player)
{
	float    max = 0.0f;
	GPlayer* other = GGame::g_game->GetNextActivePlayer(NULL);
	while (other != NULL)
	{
		if (player != other && !player->IsAllied(other))
		{
			float belief = GetBeliefInPlayer(other);
			if (belief > max)
			{
				max = belief;
			}
		}
		other = GGame::g_game->GetNextActivePlayer(other);
	}
	float belief = GetBeliefInPlayer(player);
	if (belief != 0.0f)
	{
		return max / belief;
	}
	return 0.0f;
}

void ValueSpinner::UpdatePosition(float time)
{
	point.y += time * 10.0f;
}
