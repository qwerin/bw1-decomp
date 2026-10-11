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

GBaseInfo* GBeliefInfo::GetBaseInfo(uint32_t& num_infos)
{
	num_infos = 1;
	return &Info;
}

void GBelief::Init(Town* town)
{
	for (int i = 0; i < _PLAYER_NAME_COUNT; i++)
	{
		BeliefInPlayer[i] = 0.0f;
		BeliefInPlayerMax[i] = DEFAULT_BELIEF_CAP;
		BeliefAddedTotal[i] = 0.0f;
		BeliefLossPending[i] = 0.0f;
		LastBeliefAddedTurn[i] = 0;
	}
	GPlayer* neutral = &GGame::g_game->players[GGame::g_game->NeutralPlayerIndex];
	float    neutralBelief = town->GetBeliefInNeutralPlayer();
	BeliefInPlayer[neutral->GetPlayerNumber()] = neutralBelief;
	for (int j = 0; j < NUM_REACTION_FUNCTIONS; j++)
	{
		BoredomMultiplier[j] = 1.0f;
	}
	CopyDesireAffectsBeliefAfter();
}

float GBelief::GetBeliefInPlayer(unsigned long player_number)
{
	return BeliefInPlayer[player_number];
}

float GBelief::GetBeliefInPlayer(GPlayer* player)
{
	return BeliefInPlayer[player->GetPlayerNumber()];
}

void GBelief::AddToBelief(GPlayer* player, float amount, GameThingWithPos* object, int draw_belief,
                          GUIDANCE_ALIGNMENT alignment)
{
	unsigned long playerNumber = player->GetPlayerNumber();
	BeliefAddedTotal[playerNumber] += amount;
	BeliefGained[playerNumber] += amount;
	if (amount != 0.0f)
	{
		LastBeliefAddedTurn[playerNumber] = GGame::g_game->data.GameTurn;
	}
	if (object != NULL)
	{
		if (draw_belief != 0)
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
		BeliefLossPending[playerNumber] += amount;
		if (BeliefLossPending[playerNumber] > BELIEF_LOSS_DRAW_THRESHOLD)
		{
			GameThingWithPos* centre = town->town_centre;
			DrawBelief(-BeliefLossPending[playerNumber], centre, GGame::g_game->GetPlayer(playerNumber));
			BeliefLossPending[playerNumber] = 0.0f;
		}
	}
}

float GBelief::GetBeliefInPlayerDelta(unsigned long player_number)
{
	return BeliefInPlayerDelta[player_number];
}

float GBelief::DistanceChangeToBelief(float distance, float radius)
{
	return GUtils::SigmoidThreshold(BELIEF_DISTANCE_THRESHOLD, -(distance / radius));
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
		float best = BELIEF_DIFFERENCE_FLOOR;
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
	point.y += time * VALUE_SPINNER_RISE_SPEED;
}
