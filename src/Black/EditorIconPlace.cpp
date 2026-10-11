#include "MapCellConstants.h"
#include "EditorIconPlace.h"

#include "EditorIcon.h"

EditorIconPlace::EditorIconPlace()
{
	Icon = NULL;
	field_0x8 = 0;
	X = 0;
	Y = 0;
	field_0xc = 0;
	field_0x18 = 0;
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
