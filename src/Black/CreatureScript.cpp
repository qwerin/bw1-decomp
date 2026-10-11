#include "MapCellConstants.h"
#include "CreatureAttitudeConstants.h"
#include "GameTimeConstants.h"
#include "CreatureAction.h"
#include "CreatureActionTableEntry.h"
#include "CreatureScript.h"

#include "ColourConstants.h" /* For White */
#include "Creature.h"
#include "CreatureAgenda.h"
#include "CreatureMental.h"
#include "CreatureMentalBelief.h"
#include "SubArgument.h"

#define CREATURE_SCRIPT_FILE "C:\\dev\\MP\\Black\\CreatureScript.cpp"

void Creature::SetFocus(const LHPoint& focus)
{
	mind->LookState.FocusActive = 1;
	mind->FocusIsPosition = 1;
	mind->FocusPosition = focus;
	PrepareCreatureForScriptedAction(1);
	LHPoint point = focus;
	mind->agenda.SubActionAgenda.AddSubAction(CREATURE_SUB_STATE_ACTIONS_TURN_TO_FACE_POS,
	                                          new (CREATURE_SCRIPT_FILE, 40) SubArgumentPoint(point),
	                                          &Creature::LookAtPosition, NULL);
}

void Creature::SetFocus(GameThingWithPos* thing)
{
	mind->LookState.FocusActive = 1;
	mind->FocusIsPosition = 0;
	mind->FocusObject = thing;
	PrepareCreatureForScriptedAction(1);
	CreatureBelief* belief = mind->AddBeliefAboutObject(this, thing);
	mind->agenda.SubActionAgenda.AddSubAction(
		CREATURE_SUB_STATE_ACTIONS_TURN_TO_FACE_OBJECT,
		new (CREATURE_SCRIPT_FILE, 50) SubArgumentObjectAndFloat(belief, CREATURE_SCRIPT_FOCUS_OBJECT_ARGUMENT), NULL,
		NULL);
}

void Creature::ScriptPlayIndividualAnimation(unsigned long anim)
{
	PrepareCreatureForScriptedAction(1);
	for (uint32_t i = 0; i < ScriptAnimRepeats; i++)
	{
		mind->agenda.SubActionAgenda.AddSubAction(CREATURE_SUB_STATE_ACTIONS_INDIVIDUAL_ACTION,
		                                          new (CREATURE_SCRIPT_FILE, 127) SubArgumentInteger(anim), NULL, NULL);
	}
	mind->agenda.SubActionAgenda.AddSubAction(CREATURE_SUB_STATE_ACTIONS_WAIT_UNTIL_FREE, NULL, NULL, NULL);
}

void Creature::ScriptPlayStaticAnimation(unsigned long anim, float duration)
{
	PrepareCreatureForScriptedAction(1);
	for (uint32_t i = 0; i < ScriptAnimRepeats; i++)
	{
		mind->agenda.SubActionAgenda.AddSubAction(
			CREATURE_SUB_STATE_ACTIONS_STATIC_ACTION,
			new (CREATURE_SCRIPT_FILE, 137) SubArgumentIntegerAndFloat(anim, duration), NULL, NULL);
	}
	mind->agenda.SubActionAgenda.AddSubAction(CREATURE_SUB_STATE_ACTIONS_WAIT_UNTIL_FREE, NULL, NULL, NULL);
}
