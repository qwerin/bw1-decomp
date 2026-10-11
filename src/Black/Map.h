#ifndef BW1_DECOMP_MAP_INCLUDED_H
#define BW1_DECOMP_MAP_INCLUDED_H

#include <assert.h> /* For static_assert */
#include <stdint.h> /* For uint16_t, uint32_t, uint8_t */

#include <chlasm/Enum.h> /* For enum OBJECT_TYPE */

#include <Lionhead/LHLib/ver5.0/LHListHead.h> /* For LHListHead */

#include "Base.h"      /* For struct Base */
#include "MapCoords.h" /* For enum COLLIDE_TYPE */

// Forward Declares

class Fixed;
class Object;
struct LH3DColor;
struct MapCellIterator;

#define MAP_CELLS_PER_SIDE      0x200
#define MAP_CELL_OUTLINE_HEIGHT 0.3f

enum MAP_CELL_CORNER
{
	MAP_CELL_CORNER_00 = 0,
	MAP_CELL_CORNER_10 = 1,
	MAP_CELL_CORNER_11 = 2,
	MAP_CELL_CORNER_01 = 3,
	MAP_CELL_CORNER_COUNT = 4,
};

struct MapCell
{
	Object* FirstObjectMobile; /* 0x0 */
	Object* FirstObjectFixed;

	// Static methods

	// BW1W120 00601510 BW1M119 0100cb60
	static bool32_t DoesObjectTypeCountAsFixed(OBJECT_TYPE type);

	// Constructors

	// BW1W120 inlined BW1M119 01560980
	MapCell()
	{
		FirstObjectFixed = NULL;
		FirstObjectMobile = NULL;
	}

	// BW1W120 00601150 BW1M119 01568ee0
	~MapCell();

	// Non-virtual methods

	// BW1W120 00601b60 BW1M119 01056a30
	void SetFirstObjectMobile(Object* object);
	// BW1W120 00601b70 BW1M119 015c0530
	void SetFirstObjectFixed(Object* object);
	// BW1W120 00601380 BW1M119 0156aec0
	void Clean();
	// BW1W120 006015e0 BW1M119 01014dc0
	Object* FindTypeOnMap(OBJECT_TYPE type, Object* object) const;
	// BW1W120 00601b80 BW1M119 01056e70
	uint32_t GetX() const;
	// BW1W120 00601ba0 BW1M119 01056fe0
	uint32_t GetZ() const;
	// BW1W120 00601690 BW1M119 01568c20
	Fixed* FindFixedOnMap(Object* object);
	// BW1W120 inlined BW1M119 013dcd10
	MapCellIterator GetFirstIterator() const;
	// BW1W120 00601880 BW1M119 015687d0
	void ShowMapCell(LH3DColor* color);
	// BW1W120 00601bc0 BW1M119 0111edf0
	int Collide(COLLIDE_TYPE type);
	// BW1W120 00601bd0 BW1M119 010345f0
	int Collide();
	// BW1W120 00601e00 BW1M119 0102d430
	bool32_t InBounds() const;
	// BW1W120 00601ea0 BW1M119 01301820
	bool32_t IsFixed();
};

struct MapCellIterator
{
	Object*        object; /* 0x0 */
	bool32_t       IsFixed;
	const MapCell* cell;

	// Non-virtual methods

	// BW1W120 inlined BW1M119 0102f190
	void MoveToMobileObsIfNeededAndPoss()
	{
		if (object == NULL && IsFixed)
		{
			object = cell->FirstObjectMobile;
			IsFixed = 0;
		}
	}
};

inline MapCellIterator MapCell::GetFirstIterator() const
{
	MapCellIterator iter;
	iter.object = FirstObjectFixed;
	iter.IsFixed = 1;
	iter.cell = this;
	iter.MoveToMobileObsIfNeededAndPoss();
	return iter;
}

static_assert(sizeof(MapCellIterator) == 0xc, "MapCellIterator size is incorrect");

class GMap : public Base
{
public:
	// BW1W120 006014c0 BW1M119 011c6cb0
	bool32_t         Init(unsigned long x_size, unsigned long z_size, unsigned long flags);
	uint8_t          Dirty : 1; /* 0x8 */
	uint8_t          Initialised : 1;
	uint8_t          field_0x9;
	uint8_t          field_0xa;
	uint8_t          field_0xb;
	uint32_t         CellExtentZx[0x2];
	uint32_t         field_0x14;
	uint32_t         field_0x18;
	uint32_t         field_0x1c;
	uint32_t         field_0x20;
	uint32_t         field_0x24;
	uint32_t         field_0x28;
	uint32_t         field_0x2c;
	uint32_t         field_0x30;
	uint32_t         field_0x34;
	uint32_t         field_0x38;
	uint8_t          field_0x3c;
	uint8_t          field_0x3d;
	uint8_t          field_0x3e;
	uint8_t          field_0x3f;
	uint32_t         field_0x40;
	MapCell          cells[MAP_CELLS_PER_SIDE][MAP_CELLS_PER_SIDE];
	uint16_t         InfluenceX;
	uint8_t          field_0x200046;
	uint8_t          field_0x200047;
	LHListHead<Base> field_0x200048;

	// Override methods

	// BW1W120 006010c0 BW1M119 01568da0
	virtual ~GMap();

	// Constructors

	// BW1W120 00601080 BW1M119 015697d0
	GMap();

	// Non-virtual methods

	// BW1W120 006013d0 BW1M119 01562a20
	void Clean();
	// BW1W120 00601820 BW1M119 01568560
	void CalculateMapInfluence();
	// BW1W120 006016d0 BW1M119 0155b770
	void UpdateControlMap();
	// BW1W120 00601850 BW1M119 01090d60
	void CalculateMapInfluenceX();
	// BW1W120 00612690 BW1M119 0104c5d0
	bool32_t InBounds(long x, long z) const
	{
		return (unsigned long)x < CellExtentZx[1] && (unsigned long)z < CellExtentZx[0];
	}
	// BW1W120 00612660 BW1M119 0150df10
	MapCell* ToMap(long cell_x, long cell_z)
	{
		if (!InBounds(cell_x, cell_z))
		{
			return NULL;
		}
		return &cells[0][cell_x * CellExtentZx[0] + cell_z];
	}
};

static_assert(sizeof(GMap) == 0x200050, "GMap size is incorrect");

#endif /* BW1_DECOMP_MAP_INCLUDED_H */
