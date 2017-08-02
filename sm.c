/******************************************************************************/
/**
*	sm.c
*	
*	Description:
*	State Machine Library implementation
*
*	@version HISTORY
*
* 	<PRE><TT>
*	Version	Author				Date		Modification 
*	1.0		C.Soporan        	    2013/05/07	First draft 
*	<TT></PRE>
*/
 
/*************************** Module Header Files ******************************/

#include <stdio.h>
#include <stdlib.h>
#include "sm.h"

static void make_transition(StateMachine *sm, int event, void* data);
static bool is_trans_state(StateMachine *sm, int state);

//TODO redesign transStates list, add trans attribute to the hashMap? (linked list) beware of the size and byte alignment
//TODO create value to naming translation for states, for logging ...
//TODO add timerStates, add timeout for transitory states

/******************************************************************************/
/**
*	sm_init()
*   Initialises the state machine, creating a hashmap for the state table
* 
*	NOTE: This function assumes that last entry in the state table is
*   { 0, 0, NULL, 0 }. If this is not the case, this function may cause the
*   application to crash!
*
*	@param StateMachine *sm     Pointer to the state machine
*	@return status
*/
int sm_init(StateMachine *sm)
{
    int i;
    int tableIndex = 0;
    int stateVal = 0;

    /* allocate memory for the hashmap */
    sm->stateTable_hashMap = malloc(sizeof(StateTableHashMap) * (sm->guardState + 1));

    if( NULL == sm->stateTable_hashMap )
    {
        return(UI_STATE_MACHINE_ERROR);
    }

    /*
     * It is possible that for a state, there are no entries in the
     * state table. 
     * In such cases, the hashmap value for that state index will be -1.
     */
    for( i = 0; i < ( sm->guardState + 1 ); i++ )
    {
        sm->stateTable_hashMap[i] = -1;
    }

    /* This assumes the last entry in the
     * state table will be zero */
    while( 0 != sm->stateTable[tableIndex].state )
    {
        /* If we have a new state, add it to hashmap */
        if( stateVal != sm->stateTable[tableIndex].state )
        {
            stateVal = sm->stateTable[tableIndex].state;
            sm->stateTable_hashMap[stateVal] = tableIndex;
        }
        tableIndex++;
    }
    return(UI_STATE_MACHINE_OK);
}

/******************************************************************************/
/**
*	sm_remove()
*	Frees the hashmap for indexing within the sm
*
*	@param StateMachine *sm     Pointer to the state machine
*	@return none
*/
void sm_remove(StateMachine *sm)
{
    if ( NULL != sm->stateTable_hashMap )
    {
        free( sm->stateTable_hashMap );
    }
}

/******************************************************************************/
/**
*	make_transition()
*	Frees the hashmap for indexing within the sm
*
*	@param StateMachine *sm     Pointer to the state machine
* *	@param int event            Current event
*	@return void *data          User data for raised event
*/
static void make_transition(StateMachine *sm, int event, void *data)
{
    int i;
    bool in_guardState = FALSE;

    i = sm->stateTable_hashMap[sm->currentState];

    /* No entry in the state table for the current state */
    if( -1 == i )
    {
        in_guardState = TRUE;
    }
    else
    {
        while( TRUE )
        {
            /* In case the event is not in the current state go to guard state */
            if( sm->currentState != sm->stateTable[i].state )
            {
                in_guardState = TRUE;
                break;
            }

            /* Execute the action for the specific event and change state */
            if(( NULL != sm->stateTable[i].event_func ) &&
               ( TRUE == sm->stateTable[i].event_func(event, data)))
            {
                if( NULL != sm->stateTable[i].action )
                {
                    sm->stateTable[i].action(data);
                }

                if ( sm->stateTable[i].nextState != sm->guardState )
                {
                    sm->currentState = sm->stateTable[i].nextState;
                }

                break;
            }
            i++;
        }
    }

    /* Execute transitions in GUARD_STATE */
    if( in_guardState )
    {
        i = sm->stateTable_hashMap[sm->guardState];

        if( -1 != i )
        {
            while( TRUE )
            {
                if( sm->guardState != sm->stateTable[i].state )
                {
                    break;
                }

                if( ( NULL != sm->stateTable[i].event_func ) &&
                    ( TRUE == sm->stateTable[i].event_func(event, data) ) )
                {
                    if( NULL != sm->stateTable[i].action )
                    {
                        sm->stateTable[i].action(data);
                    }

                    if( sm->stateTable[i].nextState != sm->guardState )
                    {
                        sm->currentState = sm->stateTable[i].nextState;
                    }

                    break;
                }
                i++;
            }
        }
    }
}

/******************************************************************************/
/**
*	is_trans_state()
*	Checks is a state is transitory.
*   The user will provide an array with transitory states.
*
*   WARNING:    This  function assumes that the transStates array of the SM
*   is either NULL, or the last entry in the array is zero. If not, then this
*   function may cause the application to crash!
* 
*	@param StateMachine *sm     Pointer to the state machine
* *	@param int event            Current event
*	@return void *data          User data for raised event
*/
bool is_trans_state(StateMachine *sm, int state)
{
    int i = 0;

    if( NULL == sm->transStates )
    {
        return(FALSE);
    }

    while( TRUE )
    {
        if( sm->transStates[i] == 0 )
        {
            return(FALSE);
        }

        if( sm->transStates[i] == state )
        {
            return(TRUE);
        }
        i++;
    }
}

/******************************************************************************/
/**
*	sm_stateTransition()
*   User function for ececuting a state transition
* 
*	@param StateMachine *sm     Pointer to the state machine
* *	@param int event            Current event
*	@return void *data          User data for raised event
*/
void sm_stateTransition(StateMachine *sm, int event, void *data)
{
    make_transition(sm, event, data);

    while( is_trans_state(sm, sm->currentState) )
    {
        make_transition(sm, event, data);
    }
}
