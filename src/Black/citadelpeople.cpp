#include "GameTimeConstants.h"
#include "CitadelPeople.h"

#include <Lionhead/LH3DLib/development/LH3DMesh.h> /* For struct LH3DMesh */
#include <Lionhead/LH3DLib/development/LHPoint.h>  /* For struct LHPoint */

#define CITADEL_MOVEMENT_MESH_FILE "data\\citadel\\engine\\movement.l3d"

LHPoint* CPController::Vertices;
void*    CPController::Triangles;

void CPController::Init()
{
	LH3DMesh* mesh = LH3DMesh::CreateFromHD(CITADEL_MOVEMENT_MESH_FILE, false);
	Init(mesh);
	mesh->Release();
}

void CPController::Close()
{
	delete Vertices;
	delete Triangles;
	Vertices = NULL;
	Triangles = NULL;
}
