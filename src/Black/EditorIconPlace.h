#ifndef BW1_DECOMP_EDITOR_ICON_PLACE_INCLUDED_H
#define BW1_DECOMP_EDITOR_ICON_PLACE_INCLUDED_H

#include <assert.h> /* For static_assert */
#include <stdint.h> /* For uint32_t */

#include <Lionhead/LH3DLib/development/LHRegion.h> /* For struct LHRegion */
#include <re_common.h>                             /* For bool32_t */

// Forward Declares

class EditorIconBase;
class EditorIconShow;

class EditorIconPlace
{
public:
	EditorIconBase* Parent; /* 0x0 */
	EditorIconShow* Icon;
	uint32_t        field_0x8;
	uint32_t        field_0xc;
	int             Y; /* 0x10 */
	int             X;
	uint32_t        field_0x18;

	// Constructors

	// BW1W120 005229c0
	EditorIconPlace();

	// Destructor

	// BW1W120 005229e0
	~EditorIconPlace();

	// Non-virtual methods

	// BW1W120 00522cb0
	bool32_t IsScrollable();
	// BW1W120 00522cc0
	LHRegion* GetPDMRegion();
	// BW1W120 00522cd0 BW1M119 012bbc20
	int GetAvailWidth();
};
static_assert(sizeof(EditorIconPlace) == 0x1c, "Data type is of wrong size");

#endif /* BW1_DECOMP_EDITOR_ICON_PLACE_INCLUDED_H */
