/* 
 * File:   AgentTest.c
 * Author: Mason Woodford (mwoodfor@ucsc.edu)
 *
 * Created on June 4, 2020, 9:37 PM
 */

//Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include "BattleBoats.h"

//CSE 13 Support Libraries
#include "BOARD.h"
#include "Agent.h"
#include "Oled.h"

//Tests to see if all transitions in the state machine are functioning correctly
int main (void) 
{
    BOARD_Init();
    AgentInit();
    char oledString[50];
    BB_Event bbEvent;
    int agentTests = 0;
    bbEvent.type = BB_EVENT_START_BUTTON;
    AgentSetState(AGENT_STATE_START);
    AgentRun(bbEvent);
    if(AgentGetState() == AGENT_STATE_CHALLENGING) {
        agentTests += 1;
    }   
    
    bbEvent.type = BB_EVENT_ACC_RECEIVED;
    AgentSetState(AGENT_STATE_CHALLENGING);
    AgentRun(bbEvent);
    if(AgentGetState() == AGENT_STATE_DEFENDING || AgentGetState() ==  
            AGENT_STATE_WAITING_TO_SEND) {
        agentTests += 1;
    } 
    
    bbEvent.type = BB_EVENT_MESSAGE_SENT;
    AgentSetState(AGENT_STATE_WAITING_TO_SEND);
    AgentRun(bbEvent);
    if(AgentGetState() == AGENT_STATE_ATTACKING) {
        agentTests += 1;
    }   
           
    bbEvent.type = BB_EVENT_RES_RECEIVED;
    AgentSetState(AGENT_STATE_ATTACKING);
    AgentRun(bbEvent);
    if(AgentGetState() == AGENT_STATE_DEFENDING) {
        agentTests += 1;
    }   
        
    bbEvent.type = BB_EVENT_SHO_RECEIVED;
    AgentSetState(AGENT_STATE_DEFENDING);
    AgentRun(bbEvent);
    if(AgentGetState() == AGENT_STATE_WAITING_TO_SEND) {
        agentTests += 1;
    }   
    
    bbEvent.type = BB_EVENT_CHA_RECEIVED;
    AgentSetState(AGENT_STATE_START);
    AgentRun(bbEvent);
    if(AgentGetState() == AGENT_STATE_ACCEPTING) {
        agentTests += 1;
    }   
    
    bbEvent.type = BB_EVENT_REV_RECEIVED;
    AgentSetState(AGENT_STATE_ACCEPTING);
    AgentRun(bbEvent);
    if(AgentGetState() == AGENT_STATE_DEFENDING || AgentGetState() ==  
            AGENT_STATE_ATTACKING) {
        agentTests += 1;
    } 
    
    bbEvent.type = BB_EVENT_RESET_BUTTON;
    AgentSetState(AGENT_STATE_ACCEPTING);
    AgentRun(bbEvent);
    if(AgentGetState() == AGENT_STATE_START) {
        agentTests += 1;
    }   
    
    bbEvent.type = BB_EVENT_RESET_BUTTON;
    AgentSetState(AGENT_STATE_DEFENDING);
    AgentRun(bbEvent);
    if(AgentGetState() == AGENT_STATE_START) {
        agentTests += 1;
    }   
    
    bbEvent.type = BB_EVENT_RESET_BUTTON;
    AgentSetState(AGENT_STATE_ATTACKING);
    AgentRun(bbEvent);
    if(AgentGetState() == AGENT_STATE_START) {
        agentTests += 1;
    }   
    
    OledClear(OLED_COLOR_BLACK);
    sprintf(oledString, "Agent Tests: %d/10\n", agentTests);
    OledDrawString(oledString);
    OledUpdate();   
      
    return 0;
}
