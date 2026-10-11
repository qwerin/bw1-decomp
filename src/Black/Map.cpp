#include "GameTimeConstants.h"
#include "MapCellConstants.h"
#include "Map.h"

#define MAP_CELL_OUTLINE_HEIGHT 0.3f
#define MAP_CELL_COLOUR_MAX     0xff

#include <stdio.h> /* For sprintf */

#include <Lionhead/LH3DLib/development/LH3DColor.h>
#include <Lionhead/LH3DLib/development/LH3DIsland.h>
#include <Lionhead/LH3DLib/development/LH3DLine.h>

#include "Fixed.h"
#include "Game.h"
#include "Object.h"

GMap::GMap()
{
	Dirty = 1;
}

void GMap::Clean()
{
	for (long x = 0; x < (long)CellExtentZx[0]; x++)
	{
		for (long z = 0; z < (long)CellExtentZx[1]; z++)
		{
			ToMap(x, z)->Clean();
		}
	}
}

MapCell::~MapCell()
{
	Object* object;
	do
	{
		object = FirstObjectMobile;
		if (object != NULL)
		{
			object->ToBeDeleted(0);
		}
	} while (object != NULL);
	do
	{
		object = FirstObjectFixed;
		if (object != NULL)
		{
			object->ToBeDeleted(0);
		}
	} while (object != NULL);
}

void MapCell::Clean()
{
	char text[0x80];

	Object* object = FirstObjectFixed;
	if (object == NULL)
	{
		object = FirstObjectMobile;
	}
	if (object != NULL)
	{
		sprintf(text, "Jeremy: vbad, Cell not clear -> %s", object->GetDebugText());
	}
	SetFirstObjectFixed(NULL);
	SetFirstObjectMobile(NULL);
}

bool32_t MapCell::DoesObjectTypeCountAsFixed(OBJECT_TYPE type)
{
	switch (type)
	{
	case OBJECT_TYPE_ABODE:
		return true;
	case OBJECT_TYPE_FOREST_TREE:
		return true;
	case OBJECT_TYPE_FIREWALL_SECTION:
		return true;
	case OBJECT_TYPE_CITADEL:
		return true;
	case OBJECT_TYPE_TERRAIN:
		return true;
	case OBJECT_TYPE_WALL_SECTION:
		return true;
	case OBJECT_TYPE_FAKE_WALL_SECTION:
		return true;
	case OBJECT_TYPE_BOUNDRY:
		return true;
	case OBJECT_TYPE_FIELD:
		return true;
	case OBJECT_TYPE_FEATURE:
		return true;
	case OBJECT_TYPE_POT:
		return true;
	case OBJECT_TYPE_FOOTBALL:
		return true;
	case OBJECT_TYPE_GROUND:
		return true;
	case OBJECT_TYPE_PRAYER:
		return true;
	case OBJECT_TYPE_SHOW_NEED:
		return true;
	case OBJECT_TYPE_PFOOTBALL:
		return true;
	case OBJECT_TYPE_MOBILE_STATIC:
		return true;
	case OBJECT_TYPE_DUMMY:
		return true;
	case OBJECT_TYPE_TOTEM_STATUE:
		return true;
	case OBJECT_TYPE_BIG_FOREST:
		return true;
	case OBJECT_TYPE_FISH_FARM:
		return true;
	case OBJECT_TYPE_REWARD:
		return true;
	case OBJECT_TYPE_SCRIPT_HIGHLIGHT:
		return true;
	case OBJECT_TYPE_WATERFALL:
		return true;
	case OBJECT_TYPE_HELP_ORB:
		return true;
	case OBJECT_TYPE_LEASH_SELECTOR:
		return true;
	case OBJECT_TYPE_ONE_OFF_SPELL:
		return true;
	case OBJECT_TYPE_FURNITURE:
		return true;
	case OBJECT_TYPE_FLOWERS:
		return true;
	case OBJECT_TYPE_ARENA_SPELL_ICON:
		return true;
	case OBJECT_TYPE_MAP_SHIELD:
		return true;
	default:
		return false;
	}
}

Object* MapCell::FindTypeOnMap(OBJECT_TYPE type, Object* object) const
{
	if (type == OBJECT_TYPE_ANY)
	{
		if (object == NULL)
		{
			if (FirstObjectFixed != NULL)
			{
				return FirstObjectFixed;
			}
			return FirstObjectMobile;
		}
		Object* child = object->GetMapChild(*this);
		if (child == NULL)
		{
			OBJECT_TYPE objectType = object->info->type;
			if (DoesObjectTypeCountAsFixed(objectType))
			{
				return FirstObjectMobile;
			}
		}
		return child;
	}
	Object* current;
	if (object == NULL)
	{
		current = DoesObjectTypeCountAsFixed(type) ? FirstObjectFixed : FirstObjectMobile;
	}
	else
	{
		current = object->GetMapChild(*this);
	}
	while (current != NULL)
	{
		if (current->info->type == type)
		{
			return current;
		}
		current = current->GetMapChild(*this);
	}
	return NULL;
}

Fixed* MapCell::FindFixedOnMap(Object* param_1)
{
	Object* current;
	if (param_1 == NULL)
	{
		current = FirstObjectFixed;
	}
	else
	{
		current = param_1->GetMapChild(*this);
	}
	while (current != NULL && (current->Flags & GAME_THING_WITH_POS_FLAG_FIXED) == 0)
	{
		current = current->GetMapChild(*this);
	}
	return (Fixed*)current;
}

void GMap::CalculateMapInfluence()
{
	Count0x200044 = 0;
	for (int i = 0; i < (int)CellExtentZx[1]; i++)
	{
		CalculateMapInfluenceX();
	}
}

void GMap::CalculateMapInfluenceX()
{
	Count0x200044++;
	if (Count0x200044 >= CellExtentZx[0])
	{
		Count0x200044 = 0;
	}
}

void MapCell::SetFirstObjectMobile(Object* object)
{
	FirstObjectMobile = object;
}

void MapCell::SetFirstObjectFixed(Object* object)
{
	FirstObjectFixed = object;
}

uint32_t MapCell::GetX() const
{
	return (this - &GGame::g_game->map.cells[0][0]) / GGame::g_game->map.CellExtentZx[0];
}

uint32_t MapCell::GetZ() const
{
	return (this - &GGame::g_game->map.cells[0][0]) % GGame::g_game->map.CellExtentZx[0];
}

int MapCell::Collide(COLLIDE_TYPE type)
{
	return Collide() & type;
}

bool32_t MapCell::InBounds() const
{
	uint32_t z = GetZ();
	uint32_t x = GetX();
	return x < GGame::g_game->map.CellExtentZx[1] && z < GGame::g_game->map.CellExtentZx[0];
}

bool32_t MapCell::IsFixed()
{
	if (FirstObjectFixed != NULL)
	{
		return FirstObjectFixed->IsFixed();
	}
	return false;
}

void MapCell::ShowMapCell(LH3DColor* color)
{
	LH3DColor white;
	if (color == NULL)
	{
		white.b = MAP_CELL_COLOUR_MAX;
		white.g = MAP_CELL_COLOUR_MAX;
		white.r = MAP_CELL_COLOUR_MAX;
		white.a = MAP_CELL_COLOUR_MAX;
		color = &white;
	}
	LH3DColor lineColor = *color;

	float   z = GetZ() * MetresPerMapCell;
	LHPoint corners[MAP_CELL_CORNER_COUNT];
	corners[0].x = GetX() * MetresPerMapCell;
	corners[0].y = 0.0f;
	corners[0].z = z;
	corners[2].x = corners[1].x = MetresPerMapCell + corners[0].x;
	corners[1].y = 0.0f;
	corners[1].z = z;
	corners[2].y = 0.0f;
	corners[3].z = corners[2].z = MetresPerMapCell + z;
	corners[3].x = corners[0].x;
	corners[3].y = 0.0f;
	corners[0].y = LH3DIsland::GetAltitude(corners[0]) + MAP_CELL_OUTLINE_HEIGHT;
	corners[1].y = LH3DIsland::GetAltitude(corners[1]) + MAP_CELL_OUTLINE_HEIGHT;
	corners[2].y = LH3DIsland::GetAltitude(corners[2]) + MAP_CELL_OUTLINE_HEIGHT;
	corners[3].y = LH3DIsland::GetAltitude(corners[3]) + MAP_CELL_OUTLINE_HEIGHT;
	LH3DLine::AddLine(corners[0], corners[1], &lineColor, NULL);
	LH3DLine::AddLine(corners[1], corners[2], &lineColor, NULL);
	LH3DLine::AddLine(corners[2], corners[3], &lineColor, NULL);
	LH3DLine::AddLine(corners[3], corners[0], &lineColor, NULL);
}
