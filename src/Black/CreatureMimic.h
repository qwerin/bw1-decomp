#ifndef BW1_DECOMP_CREATURE_MIMIC_INCLUDED_H
#define BW1_DECOMP_CREATURE_MIMIC_INCLUDED_H

#include <assert.h> /* For static_assert */
#include <stdint.h> /* For uint32_t */

#include <chlasm/CreatureEnum.h> /* For enum DETECTED_PLAYER_ACTION */
#include <chlasm/Enum.h>         /* For enum MAGIC_TYPE */
#include <re_common.h>           /* For bool32_t */

#include "Base.h"      /* For struct Base */
#include "MapCoords.h" /* For struct MapCoords */

// Forward Declares

class GameThingWithPos;

#define MIMIC_STOLEN_OBJECT_CITADEL_RANGE 80.0f
#define MIMIC_STAGE_LIMIT_RANDOM_RANGE    1

class CreatureMimicState : public Base
{
public:
	bool32_t               Active;
	bool32_t               Completed;
	DETECTED_PLAYER_ACTION DetectedPlayerAction; /* 0x10 */
	MAGIC_TYPE             magic_type;
	GameThingWithPos*      game_thing;
	uint32_t               ObjectMimicType;
	uint32_t               Stage;
	uint32_t               StageProgress;
	uint32_t               StageLimit;
	uint32_t               StartTurn;
	MapCoords              coords;

	// Override methods

	// BW1W120 004e9d40 BW1M119 0124a7f0
	virtual ~CreatureMimicState();

	// Constructors

	// BW1W120 004e9d20 BW1M119 01273cb0
	CreatureMimicState();

	// Non-virtual methods

	// BW1W120 004e9d60 BW1M119 01273b20
	void SetCreatureIntoStateOfMimicking(DETECTED_PLAYER_ACTION action, GameThingWithPos* thing, MAGIC_TYPE magic_type);
};

#endif /* BW1_DECOMP_CREATURE_MIMIC_INCLUDED_H */
