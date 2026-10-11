#ifndef BW1_DECOMP_CITADEL_PEOPLE_INCLUDED_H
#define BW1_DECOMP_CITADEL_PEOPLE_INCLUDED_H

// Forward Declares

struct LH3DMesh;

class CPController
{
public:
	static void* field_0xc5e3f4;
	static void* field_0xc5e3f0;

	// BW1W120 00469990 BW1M119 011cc440
	static void Init();
	// BW1W120 004699c0 BW1M119 011cbf20
	static void Init(LH3DMesh* mesh);
	// BW1W120 00469960 BW1M119 011cc4c0
	static void Close();
};

#endif /* BW1_DECOMP_CITADEL_PEOPLE_INCLUDED_H */
