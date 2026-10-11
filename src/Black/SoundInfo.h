#ifndef BW1_DECOMP_SOUND_INFO_INCLUDED_H
#define BW1_DECOMP_SOUND_INFO_INCLUDED_H

#include <assert.h> /* For static_assert */
#include <stdint.h> /* For uint32_t */

#include "BaseInfo.h"    /* For struct GBaseInfo */
#include "InfoLoaders.h" /* For INFO_DATA_BLOCK */

// Forward Declares

class Base;

class GSoundInfo : public GBaseInfo
{
public:
	uint8_t field_0x10[0xc];
	float   Radius;
	uint8_t field_0x20[0x4];
	float   FullVolumeHeight;
	float   ZeroVolumeHeight;
	float   AirStartHeight;
	float   AirPeakHeight;
	float   AirEndHeight;
	float   field_0x38;
	float   TownMusicNearDistance;
	float   TownMusicFarDistance;
	uint8_t field_0x44[0x8];

	// BW1W120 00d9a8f8
	static GSoundInfo Info;

	// Override methods

	// BW1W120 0071d6b0 BW1M119 0151b400
	virtual ~GSoundInfo();
	// BW1W120 0071d660 BW1M119 0151b3c0
	virtual GBaseInfo* GetBaseInfo(uint32_t& num_infos);

	// TODO(#377): The original declared this class in SoundMap.h.
	// Out of line: LoadBinary at 0042f750.
	INFO_DATA_BLOCK(field_0x10, field_0x44)
	INFO_ROOT_LOADERS("SoundMap.h", 19)
};

#endif /* BW1_DECOMP_SOUND_INFO_INCLUDED_H */
