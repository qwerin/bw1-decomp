#ifndef BW1_DECOMP_INFLUENCE_INCLUDED_H
#define BW1_DECOMP_INFLUENCE_INCLUDED_H

#include <assert.h> /* For static_assert */
#include <stdint.h> /* For uint32_t */

#include <chlasm/ScriptEnums.h>               /* For enum SCRIPT_OBJECT_TYPE */
#include <Lionhead/LHLib/ver5.0/LHListNode.h> /* For struct LHListNode */

#include "BaseInfo.h"         /* For struct BaseInfo */
#include "GameThingWithPos.h" /* For struct GameThingWithPos */
#include "LHPTR.h"            /* For LHPTR */

// Forward Declares

class Base;
class GPlayer;
class GameOSFile;
class LHOSFile;
class GameThing;
struct MapCoords;

enum INFL_CALC_TYPE
{
	INFL_CALC_TYPE_DEFAULT = 0,
	INFL_CALC_TYPE_ALWAYS_INCLUDE_HAND = 1,
};

#define OUTER_RING_INFLUENCE_FRACTION 0.2

class Influence
{
public:
	// BW1W120 005cd170 BW1M119 010386b0
	static float CalculatePlayerInfluence(const MapCoords& pos, GPlayer* player, int calc_flags, INFL_CALC_TYPE type,
	                                      int include_allies);
	// BW1W120 005cd230 BW1M119 010461a0
	static float CalculatePlayerRawInfluence(const MapCoords& pos, GPlayer* player, int calc_flags);
	// BW1W120 005cd400 BW1M119 inlined
	static float CalculateAllyInfluence(const MapCoords& pos, GPlayer* player);
	// BW1W120 005cd460 BW1M119 01069830
	static bool32_t IsInPlayerRawInfluence(const MapCoords& pos, GPlayer* player);
	// BW1W120 005cd490 BW1M119 010466f0
	static bool32_t IsInAntiInfluence(const MapCoords& pos, GPlayer* player);
	// BW1W120 005cd560 BW1M119 01043990
	static float CalculateInfluenceOnRange(float distance, float radius, int calc_flags);
	// BW1W120 005cd630 BW1M119 010674f0
	static GPlayer* CalculateMostInfluentialPlayer(const MapCoords& pos, float* influence);
};

class InfluenceRing : public GameThingWithPos
{
public:
	// BW1W120 005cdb90 BW1M119 0105c050
	static void ProcessRings();
	// BW1W120 005cd9d0 BW1M119 01105ad0
	static InfluenceRing* Create(const MapCoords& coords, GPlayer* player, float radius, int anti);
	// BW1W120 005cd990 BW1M119 01105bb0
	static InfluenceRing* Create(GameThingWithPos* thing, GPlayer* player, float radius, int anti);

	BaseInfo                  info;
	LHPTR<GPlayer>            player;
	float                     Influence;
	int                       Anti;
	LHListNode<InfluenceRing> next;

	// Override methods

	// BW1W120 005cda90 BW1M119 011058d0
	virtual uint32_t SaveObject(LHOSFile& file, const MapCoords* coords);
	// BW1W120 005cd8a0 BW1M119 01105da0
	virtual void ToBeDeleted(int delete_now);
	// BW1W120 0055ec40 BW1M119 010467f0
	virtual GPlayer* GetPlayer() { return player.Get(); }
	// BW1W120 0055ec10 BW1M119 01104f10
	virtual void SetPlayer(GPlayer* new_player) { player = new_player; }
	// BW1W120 0055ec60 BW1M119 011050f0
	virtual char* GetDebugText() { return "InfluenceRing:"; }
	// BW1W120 005cdd40 BW1M119 01105130
	virtual uint32_t Load(GameOSFile& file);
	// BW1W120 005cdc60 BW1M119 01105280
	virtual uint32_t Save(GameOSFile& file);
	// BW1W120 0055ec50 BW1M119 011050b0
	virtual uint32_t GetSaveType() { return GAME_THING_TYPE_INFLUENCE_RING; }
	// BW1W120 0055ebf0 BW1M119 01104eb0
	virtual void SetPos(const MapCoords& pos) { Pos = pos; }
	// BW1W120 0055ec30 BW1M119 01104f90
	virtual const char* GetText() { return "Influence Ring"; }
	// BW1W120 0055ec20 BW1M119 01104f50
	virtual bool32_t IsInfluenceRing() { return true; }
	// BW1W120 005cdc50 BW1M119 011053f0
	virtual SCRIPT_OBJECT_TYPE GetScriptObjectType();

	// Non-virtual methods

	// BW1W120 005cd900 BW1M119 01046830
	float CalculateInfluence(const MapCoords& pos, GPlayer* player, int calc_flags);
	// BW1W120 005cd960 BW1M119 01105c90
	void SetThing(GameThingWithPos* thing);
	// BW1W120 005cda60 BW1M119 01035970
	bool32_t IsInInfluence(const MapCoords& pos) const;
	// BW1W120 005cdb40 BW1M119 01105890
	void UpdateInfluence(float radius);
	// BW1W120 005cdbd0 BW1M119 inlined
	GameThingWithPos* GetThing();
	// BW1W120 005cdc00 BW1M119 010442b0
	void Process();

	// Constructors

	// BW1W120 inlined BW1M119 inlined
	InfluenceRing() {}
	// BW1W120 005cd760 BW1M119 011061e0
	InfluenceRing(const MapCoords& coords, GPlayer* player, float radius, int anti);
	// BW1W120 005cd800 BW1M119 01105fd0
	InfluenceRing(GameThingWithPos* thing, GPlayer* player, float radius, int anti);
};

#endif /* BW1_DECOMP_INFLUENCE_INCLUDED_H */
