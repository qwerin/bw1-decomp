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

PowerSpinRunner* PowerSpinRunner::First;

PowerSpinRunner* PowerSpinRunner::Create(wchar_t* text, const LHPoint& pos, const LH3DColor& colour, bool persistent)
{
	PowerSpinRunner* runner = new (POWER_SPIN_FILE, 20) PowerSpinRunner();
	runner->Init(text, pos);
	runner->Colour = colour;
	runner->Persistent = persistent;
	return persistent ? runner : NULL;
}

void PowerSpinRunner::Init(wchar_t* text, const LHPoint& pos)
{
	StartPos = pos;
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

void PowerSpin::Init(wchar_t* text, const LHPoint& pos, const LHPoint& direction, float size, float spin, int alpha)
{
	wcscpy(Text, text);
	TextLength = wcslen(Text);
	FontType = POWER_SPIN_FONT_NEUTRAL;
	LetterSpacing = POWER_SPIN_LETTER_SPACING;
	Colour = 0xffffffff;
	TrailCount = TrailHead = 1;
	FontStyle = TrailTime = 0;
	Position = pos;
	PowerSpinPos* first = Trail;
	first->Spin = spin;
	first->Size = size;
	first->Position = Position;
	first->Direction = direction;
	first->Alpha = alpha;
}

PowerSpin* PowerSpin::Update(const LHPoint& pos, const LHPoint& direction, float size, float spin, int alpha,
                             float time)
{
	TrailTime += time;
	while (TrailTime >= POWER_SPIN_TRAIL_INTERVAL)
	{
		TrailTime -= POWER_SPIN_TRAIL_INTERVAL;
		Position = pos;
		PowerSpinPos* entry = &Trail[TrailHead];
		entry->Spin = spin;
		entry->Size = size;
		entry->Position = Position;
		entry->Direction = direction;
		entry->Alpha = alpha;
		TrailHead = (TrailHead + 1) & POWER_SPIN_TRAIL_MASK;
		if (TrailCount < POWER_SPIN_TRAIL_LENGTH)
		{
			TrailCount++;
		}
	}
	return this;
}
