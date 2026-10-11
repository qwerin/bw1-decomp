#ifndef BW1_DECOMP_HAND_STATE_GRAIN_INCLUDED_H
#define BW1_DECOMP_HAND_STATE_GRAIN_INCLUDED_H

#include <assert.h> /* For static_assert */
#include <stddef.h> /* For offsetof */
#include <stdint.h> /* For uint8_t */

#include <Lionhead/LH3DLib/development/LHPoint.h> /* For struct LHPoint */

#include "GJBaseUtils.h"          /* For GJLowPassFilterVariableStep */
#include "HandStateHolding.h"     /* For struct HandStateHolding */
#include "KPSplineInterpolator.h" /* For KPSplineInterpolator */

// Forward Declares

class CHand;
class Creature;

class HandStateGrain : public HandStateHolding
{
public:
	uint8_t                              field_0xac[0x68]; /* 0xac */
	float                                Tilt;             /* 0x114 */
	float                                Raise;
	float                                PreviousTilt;
	float                                PreviousRaise;
	bool                                 field_0x124;
	int                                  field_0x128;
	float                                field_0x12c;
	float                                field_0x130;
	float                                field_0x134;
	float                                field_0x138;
	Creature*                            field_0x13c;
	KPSplineInterpolator<float>          field_0x140;
	uint32_t                             field_0x14c;
	uint8_t                              field_0x150[0x9c]; /* 0x150 */
	LHPoint                              field_0x1ec;
	uint32_t                             field_0x1f8;
	GJLowPassFilterVariableStep<LHPoint> field_0x1fc; /* 0x1fc */
	GJLowPassFilterVariableStep<float>   field_0x210;
	GJLowPassFilterVariableStep<float>   field_0x21c;

	// Override methods

	// BW1W120 005b3290 BW1M119 01341a50
	virtual void Exit();
	// BW1W120 005b2d50 BW1M119 013420f0
	virtual float GetTiltForRender();
	// BW1W120 005b2d30 BW1M119 01342150
	virtual float GetRaiseForRender();
	// BW1W120 005b32a0 BW1M119 01341930
	virtual void OverrideHandPos(LHPoint* pos);

	// Non-virtual methods

	// BW1W120 005b2f40 BW1M119 01341fb0
	void EndTimedPour();

	// Constructors

	// BW1W120 005b2b80 BW1M119 013421b0
	HandStateGrain(CHand* hand);
};
static_assert(sizeof(HandStateGrain) == 0x228, "Data type is of wrong size");

#endif /* BW1_DECOMP_HAND_STATE_GRAIN_INCLUDED_H */
