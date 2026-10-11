#include "MapCellConstants.h"
#include "GameTimeConstants.h"
#include "SoundMap.h"

#include <Lionhead/LH3DLib/development/LH3DIsland.h> /* For LH3DIsland::GetAltitude */
#include <Lionhead/LH3DLib/development/LH3DTech.h>   /* For LH3DTech::g_camera */

#include "Debug.h"
#include "Game.h"
#include "Global.h"
#include "Interface.h"
#include "SoundInfo.h"

enum
{
	MAP_COORD_FIXED_POINT_ONE = 0x10000
};

#define ATMOS_DISTANCE_INITIAL 65535.0f
#define MAP_COORDS_PER_METRE   6553.6f

#define SOUND_MAP_DEBUG_CATEGORY 1

static char* SoundSurfaceTypeNames[SOUND_SURFACE_TYPE_LAST + 1] = {
	"SOUND SURFACE TYPE NONE",       "SOUND SURFACE TYPE GRASS",         "SOUND SURFACE TYPE GRAVEL",
	"SOUND SURFACE TYPE HARD",       "SOUND SURFACE TYPE MUD",           "SOUND SURFACE TYPE SNOW",
	"SOUND SURFACE TYPE DEEP WATER", "SOUND SURFACE TYPE SHALLOW WATER", "SOUND SURFACE TYPE LOOSE FOLIAGE",
	"SOUND SURFACE TYPE LAST"};

GSoundInfo GSoundInfo::Info;

GSoundInfo::~GSoundInfo() {}

GBaseInfo* GSoundInfo::GetBaseInfo(uint32_t& param_1)
{
	param_1 = 1;
	return &Info;
}

void AtmosMapInfo::Clear()
{
	for (int i = 0; i < ATMOS_TYPE_LAST; i++)
	{
		Types[i].Count = 0;
		Types[i].Distance = ATMOS_DISTANCE_INITIAL;
	}
}

void AtmosMapTypeInfo::Add(const MapCoords& receiver, const MapCoords& coords)
{
	float dz = receiver.z * MetresPerMapCell / (float)MAP_COORD_FIXED_POINT_ONE -
	           coords.z * MetresPerMapCell / (float)MAP_COORD_FIXED_POINT_ONE;
	float dx = receiver.x * MetresPerMapCell / (float)MAP_COORD_FIXED_POINT_ONE -
	           coords.x * MetresPerMapCell / (float)MAP_COORD_FIXED_POINT_ONE;
	float distance = dx * dx + dz * dz;
	if (distance < Distance)
	{
		Distance = distance;
		NearestX = (uint16_t)(coords.x * MetresPerMapCell / (float)MAP_COORD_FIXED_POINT_ONE);
		NearestZ = (uint16_t)(coords.z * MetresPerMapCell / (float)MAP_COORD_FIXED_POINT_ONE);
	}
	Count++;
}

float GSoundMap::GetAtmosDistance(ATMOS_TYPE type)
{
	return sqrt(AtmosInfo.GetInfo(type)->Distance);
}

void GSoundMap::GetAtmosPoint(ATMOS_TYPE type, LHPoint* point)
{
	AtmosMapTypeInfo* info = AtmosInfo.GetInfo(type);
	float             z = info->NearestZ;
	float             x = info->NearestX;
	point->y = 0.0f;
	point->x = x;
	point->z = z;
}

void GSoundMap::Reset()
{
	AtmosInfo.Clear();
	Count = 0;
}

void GSoundMap::Update()
{
	CalculateRadiusPointAndDistance();
	UpdateFromMap(ReceiverCoords);
	CalculateAtmosVolumes(ReceiverCoords);
}

float GSoundMap::GetPercentage(ATMOS_TYPE type)
{
	if (Count > 0)
	{
		float atmosCount = AtmosInfo.GetInfo(type)->GetCount();
		float totalCount = Count;
		return atmosCount / totalCount;
	}
	return 0.0f;
}

void GSoundMap::AddAtmosType(ATMOS_TYPE type, const MapCoords& coords)
{
	AtmosInfo.GetInfo(type)->Add(ReceiverCoords, coords);
	if (type != ATMOS_TYPE_NONE)
	{
		Count++;
	}
}

void GSoundMap::CalculateRadiusPointAndDistance()
{
	GGame::g_game->GetCamera();
	LHPoint* receiver = &ReceiverPos;
	*receiver = LH3DTech::g_camera.pos;
	ReceiverCoords.x = (int)(receiver->x * MAP_COORDS_PER_METRE);
	ReceiverCoords.z = (int)(receiver->z * MAP_COORDS_PER_METRE);
	ReceiverCoords.altitude = 0.0f;
	ReceiverHeight = ReceiverPos.y;
	LH3DMapCoords mapCoords(receiver->x, receiver->z);
	HeightAboveLand = ReceiverHeight - LH3DIsland::GetAltitude(mapCoords);
	Radius = GSoundInfo::Info.field_0x1c;
}

float GSoundMap::GetVolumePercentageForHeight(const LHPoint& pos)
{
	LH3DMapCoords mapCoords(pos.x, pos.z);
	float         height = ReceiverPos.y - LH3DIsland::GetAltitude(mapCoords);
	if (height < GSoundInfo::Info.field_0x24)
	{
		return 1.0f;
	}
	if (height <= GSoundInfo::Info.field_0x28)
	{
		float volume =
			1.0f - (height - GSoundInfo::Info.field_0x24) / (GSoundInfo::Info.field_0x28 - GSoundInfo::Info.field_0x24);
		if (0.0f <= volume)
		{
			return volume;
		}
	}
	return 0.0f;
}

void GSoundMap::CalculateAtmosWeightings(float* param_1, float* param_2, float* param_3)
{
	float receiverHeight = ReceiverHeight;
	float heightAboveLand = HeightAboveLand;
	if (heightAboveLand < GSoundInfo::Info.field_0x24)
	{
		*param_1 = 1.0f;
	}
	else if (heightAboveLand > GSoundInfo::Info.field_0x28)
	{
		*param_1 = 0.0f;
	}
	else
	{
		*param_1 = 1.0f - (heightAboveLand - GSoundInfo::Info.field_0x24) /
		                      (GSoundInfo::Info.field_0x28 - GSoundInfo::Info.field_0x24);
		*param_1 = max(0.0f, *param_1);
	}
	if (receiverHeight < GSoundInfo::Info.field_0x24)
	{
		*param_3 = 1.0f;
	}
	else if (receiverHeight > GSoundInfo::Info.field_0x28)
	{
		*param_3 = 0.0f;
	}
	else
	{
		*param_3 = 1.0f - (receiverHeight - GSoundInfo::Info.field_0x24) /
		                      (GSoundInfo::Info.field_0x28 - GSoundInfo::Info.field_0x24);
		*param_3 = max(0.0f, *param_3);
	}
	if (heightAboveLand < GSoundInfo::Info.field_0x2c)
	{
		*param_2 = 0.0f;
	}
	else if (heightAboveLand < GSoundInfo::Info.field_0x30)
	{
		*param_2 = (heightAboveLand - GSoundInfo::Info.field_0x2c) /
		           (GSoundInfo::Info.field_0x30 - GSoundInfo::Info.field_0x2c);
		*param_2 = min(1.0f, *param_2);
	}
	else if (heightAboveLand < GSoundInfo::Info.field_0x34)
	{
		*param_2 = 1.0f - (heightAboveLand - GSoundInfo::Info.field_0x30) /
		                      (GSoundInfo::Info.field_0x34 - GSoundInfo::Info.field_0x30);
		*param_2 = max(0.0f, *param_2);
	}
	else
	{
		*param_2 = 0.0f;
	}
}

void GSoundMap::Dump()
{
	GGlobal::Global.debug.SetMessage(SOUND_MAP_DEBUG_CATEGORY, "Sound Map Calc Update X=%d Z=%d %s Count=%d",
	                                 ReceiverCoords.MapX(), ReceiverCoords.MapZ(),
	                                 SoundSurfaceTypeNames[GetSurfaceType(ReceiverCoords)], Count);
	if (GGame::g_game != NULL && GGame::g_game->MyInterface() != NULL)
	{
		MapCoords* handPos = &GGame::g_game->MyInterface()->interface_collide.pos;
		MapCoords  handCoords;
		handCoords.SetWholeX(handPos->WholeX());
		handCoords.SetWholeZ(handPos->WholeZ());
		handCoords.SetAltitude(handPos->altitude);
		GGlobal::Global.debug.SetMessage(SOUND_MAP_DEBUG_CATEGORY, "Sound Map At Hand X=%d Z=%d %s Count=%d",
		                                 handCoords.MapX(), handCoords.MapZ(),
		                                 SoundSurfaceTypeNames[GetSurfaceType(handCoords)], Count);
	}
	GGlobal::Global.debug.SetMessage(SOUND_MAP_DEBUG_CATEGORY,
	                                 "Sound Radius=%3.3f Distance=%3.3f DistanceAboveLand=%3.3f", Radius,
	                                 ReceiverHeight, HeightAboveLand);
}
