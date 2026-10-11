#include "PowerSpin.h"

#include <string.h> /* For wcscpy, wcslen */

#include <Lionhead/LHLib/ver5.0/LHWin.h> /* For operator new(size_t, const char*, uint32_t) */

#define POWER_SPIN_FILE "C:\\dev\\MP\\Black\\PowerSpin.cpp"

enum POWER_SPIN_FONT
{
	POWER_SPIN_FONT_DEFAULT,
	POWER_SPIN_FONT_GOOD,
	POWER_SPIN_FONT_NEUTRAL,
	POWER_SPIN_FONT_EVIL
};

enum
{
	POWER_SPIN_TRAIL_MASK = POWER_SPIN_TRAIL_LENGTH - 1
};

#define POWER_SPIN_INITIAL_FIELD_0X94 0.2f
#define POWER_SPIN_TRAIL_DURATION     1.0f
#define POWER_SPIN_RUNNER_DIRECTION_Y -1.0f

PowerSpinRunner* PowerSpinRunner::First;

PowerSpinRunner* PowerSpinRunner::Create(wchar_t* text, const LHPoint& pos, const LH3DColor& colour, bool param_4)
{
	PowerSpinRunner* runner = new (POWER_SPIN_FILE, 20) PowerSpinRunner();
	runner->Init(text, pos);
	runner->Colour = colour;
	runner->field_0x9bc = param_4;
	return param_4 ? runner : NULL;
}

void PowerSpinRunner::Init(wchar_t* text, const LHPoint& pos)
{
	field_0x9c0 = pos;
	PowerSpin::Init(text, pos, LHPoint(0.0f, POWER_SPIN_RUNNER_DIRECTION_Y, 0.0f), 0.0f, 0.0f, 0);
	Time = 0.0f;
}

PowerSpinRunner::PowerSpinRunner()
{
	Next = First;
	First = this;
}

PowerSpinRunner::~PowerSpinRunner()
{
	if (First == this)
	{
		First = Next;
		return;
	}
	for (PowerSpinRunner* runner = First; runner != NULL && runner->Next != NULL; runner = runner->Next)
	{
		if (runner->Next == this)
		{
			runner->Next = Next;
			return;
		}
	}
}

void PowerSpin::Init(wchar_t* text, const LHPoint& pos, const LHPoint& direction, float param_4, float param_5,
                     int param_6)
{
	wcscpy(Text, text);
	TextLength = wcslen(Text);
	FontType = POWER_SPIN_FONT_NEUTRAL;
	field_0x94 = POWER_SPIN_INITIAL_FIELD_0X94;
	Colour = 0xffffffff;
	TrailCount = TrailHead = 1;
	field_0x8c = TrailTime = 0;
	Position = pos;
	PowerSpinPos* first = Trail;
	first->field_0x1c = param_5;
	first->field_0x18 = param_4;
	first->Position = Position;
	first->Direction = direction;
	first->field_0x20 = param_6;
}

PowerSpin* PowerSpin::Update(const LHPoint& pos, const LHPoint& direction, float param_3, float param_4, int param_5,
                             float time)
{
	TrailTime += time;
	while (TrailTime >= POWER_SPIN_TRAIL_DURATION)
	{
		TrailTime -= POWER_SPIN_TRAIL_DURATION;
		Position = pos;
		PowerSpinPos* entry = &Trail[TrailHead];
		entry->field_0x1c = param_4;
		entry->field_0x18 = param_3;
		entry->Position = Position;
		entry->Direction = direction;
		entry->field_0x20 = param_5;
		TrailHead = (TrailHead + 1) & POWER_SPIN_TRAIL_MASK;
		if (TrailCount < POWER_SPIN_TRAIL_LENGTH)
		{
			TrailCount++;
		}
	}
	return this;
}
