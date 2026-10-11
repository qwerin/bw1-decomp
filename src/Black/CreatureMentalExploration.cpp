#include "GameTimeConstants.h"
#include "CreatureExplorationMap.h"
#include "CreatureMentalEditor.h"

enum
{
	EXPLORATION_CELLS_PER_REGION_SHIFT = 3,
	EXPLORATION_MAX_TIME_IN_REGION = 0xfffe
};

CreatureMentalEditor::CreatureMentalEditor(CreatureMental* mental) {}

CreatureMentalEditor::~CreatureMentalEditor() {}

void CreatureMentalEditor::Draw() {}

void CreatureMentalEditor::DrawTextA(const char* text, unsigned long x, unsigned long y, float size, unsigned long red,
                                     unsigned long green, unsigned long blue)
{
}

void CreatureMentalEditor::DrawLine(unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2,
                                    LHPixel16& colour)
{
}

void CreatureMentalEditor::Update() {}

bool32_t CreatureMentalEditor::ProcessHand(const MapCoords& coords, int param_2, int param_3)
{
	return true;
}

CreatureExplorationMap::CreatureExplorationMap(const MapCoords& coords)
{
	for (uint32_t i = 0; i < EXPLORATION_REGIONS_PER_SIDE; i++)
	{
		for (uint32_t j = 0; j < EXPLORATION_REGIONS_PER_SIDE; j++)
		{
			short x = (short)i;
			short z = (short)j;
			Visited[x][z / EXPLORATION_BITS_PER_BYTE] &= ~(1 << (z % EXPLORATION_BITS_PER_BYTE));
			Known[x][z / EXPLORATION_BITS_PER_BYTE] &= ~(1 << (z % EXPLORATION_BITS_PER_BYTE));
			TimeInRegion[x][z] = 0;
		}
	}
	this->coords = coords;
	field_0x8 = 0;
}

CreatureGlobalExplorationMap CreatureGlobalExplorationMap::GlobalMap;

CreatureExplorationRegionEntry::~CreatureExplorationRegionEntry() {}

CreatureGlobalExplorationMap::~CreatureGlobalExplorationMap() {}

CreatureExplorationMap::~CreatureExplorationMap() {}

void CreatureExplorationMap::Dump() {}

void CreatureGlobalExplorationMap::Dump() {}

int CreatureExplorationMap::KnowsAbout(short x, short z)
{
	return Known[x][z / EXPLORATION_BITS_PER_BYTE] & (1 << (z % EXPLORATION_BITS_PER_BYTE));
}

void CreatureExplorationMap::UpdateMap(const MapCoords& coords, Creature* creature)
{
	short x = coords.MapX() >> EXPLORATION_CELLS_PER_REGION_SHIFT;
	short z = coords.MapZ() >> EXPLORATION_CELLS_PER_REGION_SHIFT;
	Known[x][z / EXPLORATION_BITS_PER_BYTE] |= 1 << (z % EXPLORATION_BITS_PER_BYTE);
	Visited[x][z / EXPLORATION_BITS_PER_BYTE] |= 1 << (z % EXPLORATION_BITS_PER_BYTE);
	if (TimeInRegion[x][z] < EXPLORATION_MAX_TIME_IN_REGION)
	{
		TimeInRegion[x][z]++;
	}
}

void CreatureGlobalExplorationMap::PrecalculateMap()
{
	uint32_t x;
	uint32_t z;
	field_0x1c008 = 0;
	for (x = 0; x < EXPLORATION_REGIONS_PER_SIDE; x++)
	{
		for (z = 0; z < EXPLORATION_REGIONS_PER_SIDE; z++)
		{
			CalculateTypeForRegion(x, z);
		}
	}
	for (x = 0; x < EXPLORATION_REGIONS_PER_SIDE; x++)
	{
		for (z = 0; z < EXPLORATION_REGIONS_PER_SIDE; z++)
		{
			CalculateMaxAltitudeForRegion(x, z);
		}
	}
}
