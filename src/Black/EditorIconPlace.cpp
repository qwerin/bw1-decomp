#include "MapCellConstants.h"
#include "EditorIconPlace.h"

#include "EditorIcon.h"

EditorIconPlace::EditorIconPlace()
{
	Icon = NULL;
	Row = 0;
	X = 0;
	Y = 0;
	Column = 0;
	Flags = 0;
}

EditorIconPlace::~EditorIconPlace()
{
	delete Icon;
}

bool32_t EditorIconPlace::IsScrollable()
{
	return Parent->IsScrollable();
}

LHRegion* EditorIconPlace::GetPDMRegion()
{
	return Parent->GetPDMRegion();
}

int EditorIconPlace::GetAvailWidth()
{
	return GetPDMRegion()->end.x - X + 1;
}
