#ifndef BW1_DECOMP_SOUND_MAP_INCLUDED_H
#define BW1_DECOMP_SOUND_MAP_INCLUDED_H

#include <assert.h> /* For static_assert */
#include <stdint.h> /* For uint16_t, uint32_t, uint8_t */

#include <Lionhead/LH3DLib/development/LHPoint.h> /* For struct LHPoint */

#include "Base.h"      /* For struct Base */
#include "MapCoords.h" /* For struct MapCoords */
#include <string.h>

enum ATMOS_TYPE
{
	ATMOS_TYPE_NONE = 0,
	ATMOS_TYPE_LAST = 14
};

struct AtmosMapTypeInfo
{
	uint16_t Count; /* 0x0 */
	float    Distance;
	uint16_t NearestX;
	uint16_t NearestZ;

	// Non-virtual methods

	// BW1W120 0071d510 BW1M119 01048180
	void Add(const MapCoords& receiver, const MapCoords& coords);
	// BW1W120 inlined BW1M119 0102bbf0
	uint16_t GetCount() const { return Count; }
};
static_assert(sizeof(AtmosMapTypeInfo) == 0xc, "Data type is of wrong size");

struct AtmosMapInfo
{
	AtmosMapTypeInfo Types[ATMOS_TYPE_LAST]; /* 0x0 */

	// Non-virtual methods

	// BW1W120 0071d4f0 BW1M119 0101b180
	void Clear();
	// BW1W120 inlined BW1M119 0102b5d0
	AtmosMapTypeInfo* GetInfo(ATMOS_TYPE type)
	{
		if (type < 0 || type >= ATMOS_TYPE_LAST)
		{
			return Types;
		}
		return &Types[type];
	}
};
static_assert(sizeof(AtmosMapInfo) == 0xa8, "Data type is of wrong size");

class GSoundMap : public Base
{
public:
	AtmosMapInfo AtmosInfo; /* 0x8 */
	uint16_t     Count;
	uint32_t     field_0xb4[ATMOS_TYPE_LAST];
	LHPoint      ReceiverPos; /* 0xec */
	MapCoords    ReceiverCoords;
	float        Radius;
	float        HeightAboveLand;
	float        ReceiverHeight;

	// Override methods

	// BW1W120 0054ba10 BW1M119 013d88d0
	virtual ~GSoundMap();
	// BW1W120 0071d990 BW1M119 0106aa90
	virtual void Dump();

	// Non-virtual methods
	// BW1W120 0071d6f0 BW1M119 0107b310
	void Update();
	// BW1W120 0071d800 BW1M119 0106d0e0
	void CalculateRadiusPointAndDistance();
	// BW1W120 0071d720 BW1M119 01045d30
	void UpdateFromMap(const MapCoords& coords);
	// BW1W120 0071d8e0 BW1M119 0106a530
	static int GetSurfaceType(const MapCoords& coords);
	// BW1W120 0071dd60 BW1M119 0102b630
	void CalculateAtmosVolumes(const MapCoords& coords);
	// BW1W120 0071dac0 BW1M119 01074af0
	float GetVolumePercentageForHeight(const LHPoint& pos);
	// BW1W120 0071db80 BW1M119 0101aec0
	void CalculateAtmosWeightings(float* param_1, float* param_2, float* param_3);
	// BW1W120 0071d5c0 BW1M119 0101fd20
	float GetAtmosDistance(ATMOS_TYPE type);
	// BW1W120 0071d5e0 BW1M119 01026db0
	void GetAtmosPoint(ATMOS_TYPE type, LHPoint* point);
	// BW1W120 0071e210 BW1M119 0101b230
	float GetPercentage(ATMOS_TYPE type);
	// BW1W120 0071e260 BW1M119 01045ef0
	void AddAtmosType(ATMOS_TYPE type, const MapCoords& coords);
	// BW1W120 inlined BW1M119 01362dd0
	LHPoint GetReceiverPos() { return ReceiverPos; }

	// Constructors

	// BW1W120 0054b9d0 BW1M119 inlined
	GSoundMap();
	// BW1W120 0071d6d0 BW1M119 0101b120
	void Reset();
};

#endif /* BW1_DECOMP_SOUND_MAP_INCLUDED_H */
