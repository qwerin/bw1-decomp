#ifndef BW1_DECOMP_CREATURE_EXPLORATION_MAP_INCLUDED_H
#define BW1_DECOMP_CREATURE_EXPLORATION_MAP_INCLUDED_H

#include <assert.h> /* For static_assert */
#include <limits.h> /* For CHAR_BIT */
#include <stdint.h> /* For uint16_t, uint32_t */

#include "Base.h"      /* For struct Base */
#include "MapCoords.h" /* For struct MapCoords */

#define EXPLORATION_REGIONS_PER_SIDE       64
#define EXPLORATION_CELLS_PER_REGION_SHIFT 3
#define EXPLORATION_MAX_TIME_IN_REGION     0xfffe

enum REGION_TYPE
{
	REGION_TYPE_TOWN = 1,
	REGION_TYPE_COAST = 4,
	REGION_TYPE_SEA = 5,
	REGION_TYPE_HILL = 6,
	REGION_TYPE_7 = 7,
};

enum PREFERENCE_THAT_REGION_HAS_NOT_BEEN_VISITED
{
	PREFERENCE_THAT_REGION_HAS_NOT_BEEN_VISITED_0 = 0,
	PREFERENCE_THAT_REGION_HAS_NOT_BEEN_VISITED_1 = 1,
};

class Creature;

class CreatureExplorationMap : public Base
{
public:
	uint32_t  Flags;
	MapCoords coords;
	uint8_t   Visited[EXPLORATION_REGIONS_PER_SIDE][EXPLORATION_REGIONS_PER_SIDE / CHAR_BIT]; /* 0x18 */
	uint8_t   Known[EXPLORATION_REGIONS_PER_SIDE][EXPLORATION_REGIONS_PER_SIDE / CHAR_BIT];
	uint16_t  TimeInRegion[EXPLORATION_REGIONS_PER_SIDE][EXPLORATION_REGIONS_PER_SIDE]; /* 0x418 */

	// Constructors

	// BW1W120 004df4f0 BW1M119 01265e70
	CreatureExplorationMap(const MapCoords& coords);

	// Override methods

	// BW1W120 004df5c0 BW1M119 0124af90
	virtual ~CreatureExplorationMap();
	// BW1W120 004df9b0 BW1M119 01265320
	virtual void Dump();

	// Non-virtual methods

	// BW1W120 004df6e0
	int KnowsAbout(short x, short z);
	// BW1W120 004df620 BW1M119 01265c10
	void UpdateMap(const MapCoords& coords, Creature* creature);
	// BW1W120 004df720 BW1M119 01265370
	int FindNearest(REGION_TYPE type, const MapCoords& pos, MapCoords* nearest,
	                PREFERENCE_THAT_REGION_HAS_NOT_BEEN_VISITED preference, int param_5);
};

class CreatureExplorationRegionEntry : public Base
{
public:
	float    MaxAltitude;
	uint32_t HighestPointX;
	uint32_t HighestPointZ;
	float    HighestPointAltitude;
	uint32_t field_0x18;

	// Constructors

	// BW1W120 inlined BW1M119 012662a0
	CreatureExplorationRegionEntry() : HighestPointX(0), HighestPointZ(0), HighestPointAltitude(0) {}

	// Override methods

	// BW1W120 004df430 BW1M119 012645d0
	virtual ~CreatureExplorationRegionEntry();
};
static_assert(sizeof(CreatureExplorationRegionEntry) == 0x1c, "Data type is of wrong size");

class CreatureGlobalExplorationMap : public Base
{
public:
	CreatureExplorationRegionEntry Regions[EXPLORATION_REGIONS_PER_SIDE * EXPLORATION_REGIONS_PER_SIDE]; /* 0x8 */
	int                            MaxAltitude;

	// Descriptive name.
	// BW1W120 00c8dc40
	static CreatureGlobalExplorationMap GlobalMap;
	// BW1W120 004df9c0 BW1M119 01265230
	void PrecalculateMap();
	// BW1W120 004dfa10 BW1M119 01264cf0
	void CalculateTypeForRegion(short x, short z);
	// BW1W120 004dfb50 BW1M119 012650c0
	void CalculateMaxAltitudeForRegion(short x, short z);

	// Constructors

	// BW1W120 inlined BW1M119 inlined
	CreatureGlobalExplorationMap() : MaxAltitude(0) {}

	// Override methods

	// BW1W120 004df450 BW1M119 01264510
	virtual ~CreatureGlobalExplorationMap();
	// BW1W120 004dfbd0 BW1M119 01264c90
	virtual void Dump();
};
static_assert(sizeof(CreatureGlobalExplorationMap) == 0x1c00c, "Data type is of wrong size");

#endif /* BW1_DECOMP_CREATURE_EXPLORATION_MAP_INCLUDED_H */
