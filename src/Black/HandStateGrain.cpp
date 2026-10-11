#include "MapCellConstants.h"
#include "CreatureAttitudeConstants.h"
#include "HandStateGrain.h"

#include "ColourConstants.h" /* For White */
#include "Creature.h"
#include "CreatureMorph.h"
#include "Game.h"

enum
{
	GRAIN_SPLINE_KEY_POINT_COUNT = 8
};

#define GRAIN_ENVELOPE_ATTACK_END    0.2f
#define GRAIN_ENVELOPE_RELEASE_START 0.8f

HandStateGrain::HandStateGrain(CHand* hand) : HandStateHolding(hand)
{
	field_0x14c = 0;
	Tilt = 0.0f;
	Raise = 0.0f;
	PreviousTilt = 0.0f;
	PreviousRaise = 0.0f;
	field_0x13c = NULL;
	field_0x1f8 = 0;
	field_0x124 = false;
	field_0x12c = 0.0f;
	field_0x130 = 1.0f;
	field_0x134 = 0.0f;
	field_0x138 = 0.0f;

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
	field_0x140.SetFromArray(keyPoints);
}

void HandStateGrain::EndTimedPour()
{
	field_0x13c = NULL;
	field_0x1f8 = 0;
	Raise = 0.0f;
	Tilt = 0.0f;
	field_0x124 = false;
}

void HandStateGrain::Exit()
{
	field_0x124 = false;
	field_0x13c = NULL;
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
	if (field_0x13c == NULL)
	{
		if (field_0x1f8 == 0)
		{
			return;
		}
		source = &field_0x1ec;
	}
	else
	{
		LH3DCreature* creature3d = field_0x13c->GetCreature3D();
		source = &creature3d->TransformedMatrices[creature3d->HeadBone].GetPos();
	}
	*pos = *source;
}
