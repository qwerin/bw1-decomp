#ifndef BW1_DECOMP_POWER_SPIN_INCLUDED_H
#define BW1_DECOMP_POWER_SPIN_INCLUDED_H

#include <assert.h> /* For static_assert */
#include <stddef.h> /* For offsetof */
#include <Lionhead/LH3DLib/development/LH3DColor.h>
#include <Lionhead/LH3DLib/development/LHMatrix.h>
#include <Lionhead/LH3DLib/development/LHPoint.h>

#include "DrawingObject.h" /* For struct DrawingObject */

enum
{
	POWER_SPIN_TRAIL_LENGTH = 64,
	POWER_SPIN_TEXT_LENGTH = 66
};

struct PowerSpinPos
{
	LHPoint Position;  /* 0x0 */
	LHPoint Direction; /* 0xc */
	float   field_0x18;
	float   field_0x1c;
	int     field_0x20;
};
static_assert(sizeof(PowerSpinPos) == 0x24, "Data type is of wrong size");

class PowerSpin : public DrawingObject
{
public:
	wchar_t      Text[POWER_SPIN_TEXT_LENGTH]; /* 0x4 */
	int          TextLength;
	int          field_0x8c;
	int          FontType;
	float        field_0x94;
	int          TrailHead;
	int          TrailCount;
	float        TrailTime;
	PowerSpinPos Trail[POWER_SPIN_TRAIL_LENGTH]; /* 0xa4 */
	LH3DColor    Colour;                         /* 0x9a4 */
	LHPoint      Position;                       /* 0x9a8 */

	// Virtual methods

	// BW1W120 0066fcb0 BW1M119 01127140
	virtual void Init(wchar_t* text, const LHPoint& pos, const LHPoint& direction, float param_4, float param_5,
	                  int param_6);
	// BW1W120 0066fd80 BW1M119 01127000
	virtual PowerSpin* Update(const LHPoint& pos, const LHPoint& direction, float param_3, float param_4, int param_5,
	                          float time);
	// BW1W120 0066fe80 BW1M119 011268e0
	virtual void Draw();
};
static_assert(offsetof(PowerSpin, Position) == 0x9a8, "PowerSpin position offset is incorrect");

class PowerSpinRunner : public PowerSpin
{
public:
	PowerSpinRunner* Next; /* 0x9b4 */
	float            Time;
	bool             field_0x9bc;
	LHPoint          field_0x9c0;

	// BW1W120 00d4de74
	static PowerSpinRunner* First;

	// Static methods

	// BW1W120 0066f730 BW1M119 01127980
	static PowerSpinRunner* Create(wchar_t* text, const LHPoint& pos, const LH3DColor& colour, bool param_4);

	// Constructors

	// BW1W120 0066f800 BW1M119 011277f0
	PowerSpinRunner();

	// Virtual methods

	// BW1W120 0066f890 BW1M119 01127260
	virtual PowerSpinRunner* Update(LHMatrix matrix, float time);
	// BW1W120 0066f840 BW1M119 01127720
	virtual ~PowerSpinRunner();
	// BW1W120 0066f790 BW1M119 011278d0
	virtual void Init(wchar_t* text, const LHPoint& pos);
};
static_assert(offsetof(PowerSpinRunner, Next) == 0x9b4, "PowerSpinRunner link offset is incorrect");
static_assert(sizeof(PowerSpinRunner) == 0x9cc, "Data type is of wrong size");

#endif /* BW1_DECOMP_POWER_SPIN_INCLUDED_H */
