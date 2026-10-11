#ifndef BW1_DECOMP_CREATURE_MENTAL_EDITOR_INCLUDED_H
#define BW1_DECOMP_CREATURE_MENTAL_EDITOR_INCLUDED_H

#include <re_common.h> /* For bool32_t */

#include "MapCoords.h" /* For struct MapCoords */

// Forward Declares

class CreatureMental;
struct LHPixel16;

class CreatureMentalEditor
{
public:
	static CreatureMentalEditor* Instance; // 00c8dc34, descriptive identifier

	// Static methods

	// BW1W120 004df320 BW1M119 01264310
	static void DrawLine(unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2, LHPixel16& colour);
	// Mac calls this DrawText; the Windows SDK macro gives the emitted DrawTextA spelling.
	// BW1W120 004df310 BW1M119 01264360
	static void DrawTextA(const char* text, unsigned long x, unsigned long y, float size, unsigned long red,
	                      unsigned long green, unsigned long blue);
	// BW1W120 004df340 BW1M119 01093c10
	static void Update();

	// Constructors

	// BW1W120 004df2c0 BW1M119 01264470
	CreatureMentalEditor(CreatureMental* mental);
	// BW1W120 004df2d0 BW1M119 012643f0
	~CreatureMentalEditor();

	// Non-virtual methods

	// BW1W120 004df2e0 BW1M119 012643b0
	void Draw();
	// BW1W120 004df2b0 BW1M119 012644c0
	bool32_t ProcessHand(const MapCoords& coords, int, int);
};

#endif /* BW1_DECOMP_CREATURE_MENTAL_EDITOR_INCLUDED_H */
