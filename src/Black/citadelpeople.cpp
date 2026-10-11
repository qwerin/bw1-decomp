#include "GameTimeConstants.h"
#include "CitadelPeople.h"

#include <Lionhead/LH3DLib/development/LH3DMesh.h> /* For struct LH3DMesh */

#define CITADEL_MOVEMENT_MESH_FILE "data\\citadel\\engine\\movement.l3d"

void* CPController::field_0xc5e3f4;
void* CPController::field_0xc5e3f0;

void CPController::Init()
{
	LH3DMesh* mesh = LH3DMesh::CreateFromHD(CITADEL_MOVEMENT_MESH_FILE, false);
	Init(mesh);
	mesh->Release();
}

void CPController::Close()
{
	delete field_0xc5e3f4;
	delete field_0xc5e3f0;
	field_0xc5e3f4 = NULL;
	field_0xc5e3f0 = NULL;
}
