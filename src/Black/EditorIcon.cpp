#include "GameTimeConstants.h"
#include "EditorIcon.h"

#include <string.h> /* For strcpy, strlen */

#include <Lionhead/LH3DLib/development/LH3DMesh.h>   /* For LH3DMesh::MeshPack */
#include <Lionhead/LH3DLib/development/LH3DObject.h> /* For LH3DObject::SetMesh */

#include "EditorIconPlace.h"
#include "Game3DObject.h" /* For Game3DObject::Create */
#include "Object.h"       /* For Object::Get3DType */

static bool32_t ResizeRegionToText = true;

#define EDITOR_ICON_FILE "C:\\dev\\MP\\Black\\EditorIcon.cpp"

EditorIconShow::EditorIconShow(LHRegion region, const char* text, unsigned long flags, LH3DColor* active_color,
                               LH3DColor* inactive_color, LH3DColor* hilite_color, LH3DColor* text_color)
{
	Flags = flags;
	Region = region;
	Init(text, MSH_INVALID, active_color, inactive_color, hilite_color, text_color);
}

EditorIconShow::~EditorIconShow()
{
	SetG3DObject(MSH_INVALID, 0);
}

void EditorIconShow::Init(const char* text, MESH_LIST mesh, LH3DColor* active_color, LH3DColor* inactive_color,
                          LH3DColor* hilite_color, LH3DColor* text_color)
{
	Object = NULL;
	ObjectRefreshPending = false;
	Processed = false;
	TextBuffer[0] = 0;
	Active = false;
	ActiveColor = active_color != NULL ? *active_color : DefaultActiveColor;
	InactiveColor = inactive_color != NULL ? *inactive_color : DefaultInactiveColor;
	TextColor = text_color != NULL ? *text_color : DefaultTextColor;
	RightClicked = false;
	LeftClicked = false;
	SetG3DObject(mesh, 0);
	SetText(text);
	HiliteTime = 0;
}

int EditorIconShow::MakeActive()
{
	LHCoord mouse = LHSys::TheSystem.mouse.Pos();
	Active = GetRegion()->CoordInRegion(mouse);
	return Active;
}

EditorIconShow::EditorIconShow(const LHCoord& position, const char* text, unsigned long flags, LH3DColor* active_color,
                               LH3DColor* inactive_color, LH3DColor* hilite_color, LH3DColor* text_color)
{
	Flags = flags;
	Region = LHRegion(position.x, position.y, 0, EDITOR_ICON_DEFAULT_HEIGHT);
	Init(text, MSH_INVALID, active_color, inactive_color, hilite_color, text_color);
}

void EditorIconShow::SetTextToConstant(const char* text)
{
	Flags |= EDITOR_ICON_FLAG_COPY_TEXT;
	SetText(text);
}

int EditorIconShow::Process()
{
	bool32_t wasActive = Active;
	RightClicked = false;
	LeftClicked = false;
	Processed = true;
	if (MakeActive())
	{
		if (ObjectRefreshPending && !wasActive)
		{
			SetG3DObject(Mesh, 1);
		}
		if (LHSys::TheSystem.mouse.ButtonPressed & LH_MOUSE_BUTTON_LEFT_DOWN)
		{
			LeftClicked = true;
			LHSys::TheSystem.mouse.ButtonPressed &= ~LH_MOUSE_BUTTON_LEFT_DOWN;
		}
		if (LHSys::TheSystem.mouse.ButtonPressed & LH_MOUSE_BUTTON_RIGHT_DOWN)
		{
			RightClicked = true;
			LHSys::TheSystem.mouse.ButtonPressed &= ~LH_MOUSE_BUTTON_RIGHT_DOWN;
		}
		return true;
	}
	return false;
}

void EditorIconShow::GetG3DObjectRegion(LHRegion& region)
{
	long height = region.Height();
	long x = region.end.x - EDITOR_ICON_G3D_OBJECT_INSET;
	long y = region.start.y + height / 2 - EDITOR_ICON_G3D_OBJECT_VERTICAL_OFFSET;
	region.start.x = x;
	region.start.y = y;
	region.end.x = x + EDITOR_ICON_G3D_OBJECT_SIZE - 1;
	region.end.y = y + EDITOR_ICON_G3D_OBJECT_SIZE - 1;
}

void EditorIconShow::SetG3DObject(MESH_LIST mesh, int position_object)
{
	if (mesh == MSH_DUMMY)
	{
		Mesh = MSH_INVALID;
		return;
	}
	if (mesh != MSH_INVALID)
	{
		if (Object == NULL)
		{
			Object = Game3DObject::Create(::Object::Get3DType(mesh));
		}
		if (Object != NULL)
		{
			Object->SetMesh(LH3DMesh::GetPackedMesh(mesh), NULL, NULL);
			LHRegion region = *GetRegion();
			if (position_object != 0)
			{
				GetG3DObjectRegion(region);
			}
			ObjectRefreshPending = false;
		}
	}
	Mesh = mesh;
}

void EditorIconShow::Draw(bool32_t parent_active)
{
	LH3DColor* color = IsActive() ? &ActiveColor : &InactiveColor;
	DrawBox(0);
	DrawTextString(color, GetRegion());
	if (Mesh != MSH_INVALID && IsActive() && Mesh != MSH_I_OFFERING_PLAY)
	{
		SetG3DObject(Mesh, 1);
	}
}

LH3DColor* EditorIconBase::GetTextColor()
{
	return &DefaultTextColor;
}

LH3DColor* EditorIconBase::GetActiveColor()
{
	return &DefaultActiveColor;
}

LH3DColor* EditorIconBase::GetInactiveColor()
{
	return &DefaultInactiveColor;
}

char* EditorIconBase::GetText()
{
	return NULL;
}

EditorIconBool::EditorIconBool(const LHRegion& region, int& value, const char* text, const char* inactive_text,
                               unsigned long flags)
	: EditorIconShow(region, text, flags, NULL, NULL, NULL, NULL), Value(&value)
{
	InactiveText = NULL;
	SetInactiveText(inactive_text);
	if (inactive_text != NULL && LHSys::TheSystem.text.StringWidth((char*)inactive_text, NULL) >
	                                 LHSys::TheSystem.text.StringWidth((char*)text, NULL))
	{
		SetText(inactive_text);
		Text = (char*)text;
		return;
	}
	SetText(text);
}

EditorIconBool::EditorIconBool(const LHCoord& position, int& value, const char* text, const char* inactive_text,
                               unsigned long flags)
	: EditorIconShow(position, text, flags, NULL, NULL, NULL, NULL), Value(&value)
{
	InactiveText = NULL;
	SetInactiveText(inactive_text);
	if (inactive_text != NULL && LHSys::TheSystem.text.StringWidth((char*)inactive_text, NULL) >
	                                 LHSys::TheSystem.text.StringWidth((char*)text, NULL))
	{
		SetText(inactive_text);
		Text = (char*)text;
		return;
	}
	SetText(text);
}

EditorIconBool::~EditorIconBool()
{
	if (InactiveText != NULL)
	{
		SetInactiveText(NULL);
	}
}

void EditorIconBool::SetInactiveText(const char* text)
{
	if (InactiveText != NULL)
	{
		delete InactiveText;
	}
	InactiveText = NULL;
	if (text != NULL)
	{
		InactiveText = new (EDITOR_ICON_FILE, 270) char[strlen(text) + 1];
		strcpy(InactiveText, text);
	}
}

int EditorIconBool::GetValue()
{
	return *Value;
}

void EditorIconBool::SetValue(int value)
{
	*Value = value;
}

int EditorIconBool::IsActive()
{
	return Active || *Value;
}

int EditorIconBool::Process()
{
	RightClicked = false;
	LeftClicked = false;
	Processed = true;
	if (MakeActive())
	{
		uint8_t buttons = LHSys::TheSystem.mouse.ButtonPressed;
		if (buttons & LH_MOUSE_BUTTON_LEFT_DOWN)
		{
			buttons &= ~LH_MOUSE_BUTTON_LEFT_DOWN;
			LHSys::TheSystem.mouse.ButtonPressed = buttons;
			LeftClicked = true;
			SetValue(GetValue() == 0);
		}
		else if (buttons & LH_MOUSE_BUTTON_RIGHT_DOWN)
		{
			buttons &= ~LH_MOUSE_BUTTON_RIGHT_DOWN;
			LHSys::TheSystem.mouse.ButtonPressed = buttons;
			RightClicked = true;
			SetValue(GetValue() == 0);
		}
		return true;
	}
	return false;
}

void EditorIconBool::Draw(bool32_t parent_active)
{
	EditorIconShow::Draw(parent_active);
}

char* EditorIconBool::GetText()
{
	if (InactiveText != NULL && !GetValue())
	{
		return InactiveText;
	}
	return Text;
}

EditorIconTextBox::EditorIconTextBox(const LHRegion& region, char* text, LH3DColor* colour)
{
	Region = region;
	Colour = *colour;
	Text = text;
}

LHRegion* EditorIconTextBox::GetRegion()
{
	return &Region;
}

int EditorIconTextBox::Process()
{
	return true;
}

void EditorIconTextBox::Draw(bool32_t parent_active)
{
	DrawBox(0);
	DrawTextString(NULL, GetRegion());
}

int EditorIconTextBox::IsActive()
{
	return false;
}

char* EditorIconTextBox::GetText()
{
	return Text;
}

void EditorIconShow::SetRegion(EditorIconPlace& place)
{
	LHRegion* region = GetRegion();
	long      endY = region->end.y;
	long      startY = region->start.y;
	int       width = place.GetAvailWidth();
	long      y = place.Y;
	long      x = place.X;
	region = GetRegion();
	region->start.y = y;
	region->start.x = x;
	region->end.x = width + x - 1;
	region->end.y = y - startY + endY;
	region = GetRegion();
	place.Y += region->Height();
}

void EditorIconShow::SetText(const char* text)
{
	if (Flags & EDITOR_ICON_FLAG_COPY_TEXT)
	{
		if (text != TextBuffer && text != NULL)
		{
			strcpy(TextBuffer, text);
		}
		Text = TextBuffer;
	}
	else
	{
		Text = (char*)text;
	}
	if (Text != NULL && *Text != 0 && ResizeRegionToText)
	{
		int width = LHSys::TheSystem.text.StringWidth(Text, NULL);
		width += EDITOR_ICON_TEXT_PADDING;
		Region = LHRegion(Region.X1(), Region.Y1(), width, Region.Height());
	}
}

LHRegion* EditorIconRequester::GetRegion()
{
	return &Region;
}

LHRegion* EditorIconRequester::GetTextRegion()
{
	return &TextRegion;
}

int EditorIconRequester::IsActive()
{
	return Active;
}

int EditorIconRequester::Process()
{
	Active = Region.CoordInRegion(LHSys::TheSystem.mouse.Pos());
	if (CancelButton != NULL)
	{
		Active |= CancelButton->Process();
	}
	if (AcceptButton != NULL)
	{
		Active |= AcceptButton->Process();
	}
	return Active;
}

void EditorIconRequester::Draw(bool32_t parent_active)
{
	DrawBox(&Region, &Colour, EDITOR_ICON_ALPHA_OPAQUE, 0);
	DrawBubbleBox(GetRegion(), GetInactiveColor());
	if (Label != NULL)
	{
		Label->Draw(false);
	}
	if (CancelButton != NULL)
	{
		CancelButton->Draw(false);
	}
	if (AcceptButton != NULL)
	{
		AcceptButton->Draw(false);
	}
}

void EditorIconEditBox::Draw(bool32_t parent_active)
{
	if (IsActive())
	{
		DrawBox(GetRegion(), GetActiveColor(), EDITOR_ICON_ALPHA_OPAQUE, 0);
		DrawTextString(GetActiveColor(), GetRegion());
	}
	else
	{
		DrawBox(GetRegion(), GetInactiveColor(), EDITOR_ICON_ALPHA_OPAQUE, 0);
		DrawTextString(GetInactiveColor(), GetRegion());
	}
}
