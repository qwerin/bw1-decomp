#ifndef BW1_DECOMP_CREATURE_EXPLORATION_MAP_INCLUDED_H
#define BW1_DECOMP_CREATURE_EXPLORATION_MAP_INCLUDED_H

#include <assert.h> /* For static_assert */
#include <stdint.h> /* For uint16_t, uint32_t */

#include "Base.h"      /* For struct Base */
#include "MapCoords.h" /* For struct MapCoords */

enum
{
	EXPLORATION_REGIONS_PER_SIDE = 64,
	EXPLORATION_BITS_PER_BYTE = 8,
	EXPLORATION_CELLS_PER_REGION = 8
};

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
	uint32_t  field_0x8;
	MapCoords coords;
	uint8_t  Visited[EXPLORATION_REGIONS_PER_SIDE][EXPLORATION_REGIONS_PER_SIDE / EXPLORATION_BITS_PER_BYTE]; /* 0x18 */
	uint8_t  Known[EXPLORATION_REGIONS_PER_SIDE][EXPLORATION_REGIONS_PER_SIDE / EXPLORATION_BITS_PER_BYTE];
	uint16_t TimeInRegion[EXPLORATION_REGIONS_PER_SIDE][EXPLORATION_REGIONS_PER_SIDE]; /* 0x418 */

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
	uint32_t field_0x8;
	uint32_t field_0xc;
	uint32_t field_0x10;
	uint32_t field_0x14;
	uint32_t field_0x18;

	// Constructors

	// BW1W120 inlined BW1M119 012662a0
	CreatureExplorationRegionEntry() : field_0xc(0), field_0x10(0), field_0x14(0) {}

	// Override methods

	// BW1W120 004df430 BW1M119 012645d0
	virtual ~CreatureExplorationRegionEntry();
};
static_assert(sizeof(CreatureExplorationRegionEntry) == 0x1c, "Data type is of wrong size");

class CreatureGlobalExplorationMap : public Base
{
public:
	CreatureExplorationRegionEntry Regions[EXPLORATION_REGIONS_PER_SIDE * EXPLORATION_REGIONS_PER_SIDE]; /* 0x8 */
	int                            field_0x1c008;

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
	CreatureGlobalExplorationMap() : field_0x1c008(0) {}

	// Override methods

	// BW1W120 004df450 BW1M119 01264510
	virtual ~CreatureGlobalExplorationMap();
	// BW1W120 004dfbd0 BW1M119 01264c90
	virtual void Dump();
};
static_assert(sizeof(CreatureGlobalExplorationMap) == 0x1c00c, "Data type is of wrong size");

#endif /* BW1_DECOMP_CREATURE_EXPLORATION_MAP_INCLUDED_H */
