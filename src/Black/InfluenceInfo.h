#ifndef BW1_DECOMP_INFLUENCE_INFO_INCLUDED_H
#define BW1_DECOMP_INFLUENCE_INFO_INCLUDED_H

#include <assert.h> /* For static_assert */
#include <stdint.h> /* For uint32_t */

#include "BaseInfo.h"    /* For struct GBaseInfo */
#include "InfoLoaders.h" /* For INFO_DATA_BLOCK */

// Forward Declares

class Base;

class GInfluenceInfo : public GBaseInfo
{
public:
	float FullInfluenceFraction;
	float FalloffFraction;
	float FalloffLoss;

	// BW1W120 00d17cc0
	static GInfluenceInfo Info;

	// Override methods

	// BW1W120 005cd150 BW1M119 011071f0
	virtual ~GInfluenceInfo();
	// BW1W120 005cd110 BW1M119 011071a0
	virtual GBaseInfo* GetBaseInfo(uint32_t& param_1);

	// TODO(#377): The original declared this class in Influence.h.
	// Out of line: LoadBinary at 0042f700, Load at 0042f6b0.
	INFO_DATA_BLOCK(FullInfluenceFraction, FalloffLoss)
	INFO_ROOT_LOADERS("Influence.h", 19)
};

#endif /* BW1_DECOMP_INFLUENCE_INFO_INCLUDED_H */
