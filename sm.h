/******************************************************************************/
/**
*	sm.h
*	
*	Description:
*	State Machine Library interface
*   This is a basic implementation so far, it will be extended...
* 
*   The application will define two enums starting with 1 for states and events:
*   typedef enum{
*       STATE_IDLE = 1,
*       ...
*       STATE_ERROR_TRANS
*       ...
*       STATE_GUARD
*   }States;
* 
*   Notes: 
*   
*   - Transitory states are states which are always passed through but 
*   are used to do some specific work and the state machine doesn't need
*   to stay in that state (e.g. error recovery state). 
*   The transitory states will be defined also separately in an 0 ending array:
*       int transStates[] =
*       {
*           STATE_SESSION_DONE_TRANS,
*           0
*       };
*   - Value for the `guardState` in the StateMachine structure should be 
*   the last in the list (e.g STATE_GUARD, the name is user choice).
*   - The `guardState` will be entered if: 
*       - there is no entry in the state table for the current state or
*       - an event which is not in the current state was raised
* 
*   typedef enum{
*       EVENT_START = 1,
*       ...
*   }Events;
* 
*   Note: no restriction on the event enum except it starts with 1.
*   
*   The state table will be constructed like:
* 
*   StateTableEntry smTable[] =
*   {
*       // STATE,            EVENT,        ACTION,        NEXTSTATE
*       { STATE_GUARD,      event_1,      action_1,       STATE_... },
*       { STATE_GUARD,      event_2,      action_2,       STATE_... },
*           ...
* 
*	    { STATE_IDLE,       event_start,  action_start,   STATE_START },
*    
*       { STATE_START,      event_1,      action_1,       STATE_... },
*       { STATE_START,      event_2,      action_2,       STATE_... },
*           ...
* 
*       { STATE_ERROR_TRANS,    event_true,         action_recover,  STATE_IDLE },
*    
*       // State table should always end with the next line!
*	    { 0, 0, NULL, 0 }
*   };
*
*   The StateMachine definition example:
*   
*   StateMachine exampleSM =
*   {
*       STATE_IDLE,         // Inital current state
*       STATE_GUARD,        // Last state in the list
*       smTable,            // State table
*       NULL,               // State table hash map, user doesn't need to worry about this, 
*                           // this will be allocated within the library, simply intialise to NULL
*       transStates         // Transitory states array
*   };
* 
*   The user will define the each event handler function and action function.
*   Example:
*       void event_init(int event, void *data)
*       {
*           // User is resposible of how he uses the data
*           if (EVENT_START == event)
*               return TRUE;
*           else
*               return FALSE;   
*       }
*       void action(void *data)
*       {
*           // Do some work
*       }
*   Usage:
*       sm_init(&exampleSM);
*       sm_stateTransition(&exampleSM, EVENT_START, NULL);
* 
*	@version HISTORY
*
* 	<PRE><TT>
*	Version	Author			Approved By		Date		Modification 
*	1.0		C.Soporan		        		2013/07/22	First draft 
*	<TT></PRE>
*/

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

typedef enum{ 
    FALSE, 
    TRUE 
}bool;

/* error handling */

typedef enum
{
    UI_STATE_MACHINE_ERROR = -1,
    UI_STATE_MACHINE_OK = 0
} SMResult;

typedef int StateTableHashMap;

typedef struct
{
    int state;
    bool (*event_func)(int event, void*);
    void (*action)(void*);
    int nextState;
}StateTableEntry;

typedef struct
{
    int currentState;                           /* Initialized by the app */
    int guardState;                             /* End state in the enum */
    StateTableEntry *stateTable;
    StateTableHashMap *stateTable_hashMap;      /* Keep the state table index of the states */
    int *transStates;                           /* List of trasitory states */
}StateMachine;

int sm_init(StateMachine *sm);
void sm_remove(StateMachine *sm);
void sm_stateTransition(StateMachine *sm, int event, void *data);

#endif // STATEMACHINE_H
