#include "GameTimeConstants.h"
#include "CreatureExplorationMap.h"
#include "CreatureMentalEditor.h"

CreatureMentalEditor::CreatureMentalEditor(CreatureMental*) {}

CreatureMentalEditor::~CreatureMentalEditor() {}

void CreatureMentalEditor::Draw() {}

void CreatureMentalEditor::DrawTextA(const char*, unsigned long, unsigned long, float, unsigned long, unsigned long,
                                     unsigned long)
{
}

void CreatureMentalEditor::DrawLine(unsigned long, unsigned long, unsigned long, unsigned long, LHPixel16&) {}

void CreatureMentalEditor::Update() {}

bool32_t CreatureMentalEditor::ProcessHand(const MapCoords&, int, int)
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
			Visited[x][z / CHAR_BIT] &= ~(1 << (z % CHAR_BIT));
			Known[x][z / CHAR_BIT] &= ~(1 << (z % CHAR_BIT));
			TimeInRegion[x][z] = 0;
		}
	}
	this->coords = coords;
	Flags = 0;
}

CreatureGlobalExplorationMap CreatureGlobalExplorationMap::GlobalMap;

CreatureExplorationRegionEntry::~CreatureExplorationRegionEntry() {}

CreatureGlobalExplorationMap::~CreatureGlobalExplorationMap() {}

CreatureExplorationMap::~CreatureExplorationMap() {}

void CreatureExplorationMap::Dump() {}

void CreatureGlobalExplorationMap::Dump() {}

int CreatureExplorationMap::KnowsAbout(short x, short z)
{
	return Known[x][z / CHAR_BIT] & (1 << (z % CHAR_BIT));
}

void CreatureExplorationMap::UpdateMap(const MapCoords& coords, Creature*)
{
	short x = coords.MapX() >> EXPLORATION_CELLS_PER_REGION_SHIFT;
	short z = coords.MapZ() >> EXPLORATION_CELLS_PER_REGION_SHIFT;
	Known[x][z / CHAR_BIT] |= 1 << (z % CHAR_BIT);
	Visited[x][z / CHAR_BIT] |= 1 << (z % CHAR_BIT);
	if (TimeInRegion[x][z] < EXPLORATION_MAX_TIME_IN_REGION)
	{
		TimeInRegion[x][z]++;
	}
}

void CreatureGlobalExplorationMap::PrecalculateMap()
{
	uint32_t x;
	uint32_t z;
	MaxAltitude = 0;
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
