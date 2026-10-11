#include "MapCellConstants.h"
#include "CreatureAttitudeConstants.h"
#include "HandStateGrain.h"

#include "ColourConstants.h" /* For White */
#include "Creature.h"
#include "CreatureMorph.h"
#include "Game.h"

HandStateGrain::HandStateGrain(CHand* hand) : HandStateHolding(hand)
{
	TimedPourState = 0;
	Tilt = 0.0f;
	Raise = 0.0f;
	PreviousTilt = 0.0f;
	PreviousRaise = 0.0f;
	PourTarget = NULL;
	HasPourTargetPos = 0;
	Pouring = false;
	PourProgress = 0.0f;
	PourDuration = 1.0f;
	PourTilt = 0.0f;
	PourRaise = 0.0f;

	GJArray<float> keyPoints;
	keyPoints.SetSize(GRAIN_SPLINE_KEY_POINT_COUNT);
	keyPoints[0] = 0.0f;
	keyPoints[1] = 0.0f;
	keyPoints[2] = GRAIN_ENVELOPE_ATTACK_END;
	keyPoints[3] = 1.0f;
	keyPoints[4] = GRAIN_ENVELOPE_RELEASE_START;
	keyPoints[5] = 1.0f;
	keyPoints[6] = 1.0f;
	keyPoints[7] = 0.0f;
	PourEnvelope.SetFromArray(keyPoints);
}

void HandStateGrain::EndTimedPour()
{
	PourTarget = NULL;
	HasPourTargetPos = 0;
	Raise = 0.0f;
	Tilt = 0.0f;
	Pouring = false;
}

void HandStateGrain::Exit()
{
	Pouring = false;
	PourTarget = NULL;
}

float HandStateGrain::GetTiltForRender()
{
	return PreviousTilt + (Tilt - PreviousTilt) * GGame::g_game->TurnFraction;
}

float HandStateGrain::GetRaiseForRender()
{
	return PreviousRaise + (Raise - PreviousRaise) * GGame::g_game->TurnFraction;
}

void HandStateGrain::OverrideHandPos(LHPoint* pos)
{
	const LHPoint* source;
	if (PourTarget == NULL)
	{
		if (HasPourTargetPos == 0)
		{
			return;
		}
		source = &PourTargetPos;
	}
	else
	{
		LH3DCreature* creature3d = PourTarget->GetCreature3D();
		source = &creature3d->TransformedMatrices[creature3d->HeadBone].GetPos();
	}
	*pos = *source;
}
