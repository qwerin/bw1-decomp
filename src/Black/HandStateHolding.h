#ifndef BW1_DECOMP_HAND_STATE_HOLDING_INCLUDED_H
#define BW1_DECOMP_HAND_STATE_HOLDING_INCLUDED_H

#include <assert.h> /* For static_assert */
#include <stdint.h> /* For uint8_t, uintptr_t */

#include "HandState.h" /* For struct HandState */

// Forward Declares

class CHand;
struct LHPoint;

class HandStateHolding : public HandState
{
public:
	uintptr_t field_0x8;
	uintptr_t field_0xc;
	uintptr_t field_0x10;
	uint8_t   field_0x14[0x98]; /* 0x14 */

	// Override methods

	// BW1W120 005b3ac0 BW1M119 010973b0
	virtual float GetTiltForRender();
	// BW1W120 005b3ad0 BW1M119 010996d0
	virtual float GetRaiseForRender();
	// BW1W120 005b3ae0 BW1M119 01099600
	virtual void OverrideHandPos(LHPoint* pos);

	// Constructors

	// BW1W120 005b3aa0 BW1M119 01343b80
	HandStateHolding(CHand* hand);
};

#endif /* BW1_DECOMP_HAND_STATE_HOLDING_INCLUDED_H */
