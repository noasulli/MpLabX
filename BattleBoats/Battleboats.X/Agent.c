/* 
 * File:   Agent.c
 * Author: Mason Woodford(mwoodfor@ucsc.edu)
 *
 * Created on June 4, 2020, 9:37 PM
 */

#include <stdio.h>
#include <stdlib.h>

#include "Agent.h"
#include "BattleBoats.h"
#include "Oled.h"
#include "Field.h"
#include "Message.h"
#include "Negotiation.h"
#include "FieldOled.h"

//Helper Function
FieldOledTurn ChangeTurn(FieldOledTurn whosTurn);

/*
 * 
 */
static int turnCounter = 0;
static AgentState agentState = AGENT_STATE_START;
static Field myField, oppField;
static NegotiationData randomA = 0, hashA = 0, randomB = 0;
static GuessData guessResult, oppGuess;
static NegotiationOutcome coinFlip = 0;
static FieldOledTurn whosTurn = FIELD_OLED_TURN_NONE;
const char *greetingMessage = "This is BattleBoats!\nPress BTN4 to\nchallenge, "
                                                    "or wait \nfor opponent";

const char *endMessage = "Ready for a new game?\n Press BTN4 to begin";

void AgentInit(void)
{
    OledInit();
    OledClear(OLED_COLOR_BLACK);
    OledDrawString(greetingMessage);
    OledUpdate();
    agentState = AGENT_STATE_START;
    turnCounter = 0;
}

Message AgentRun(BB_Event event)
{
    int fieldSuccess;
    char oledString[100];
    Message message;
    switch (agentState) {
    case AGENT_STATE_START:
        if (event.type == BB_EVENT_RESET_BUTTON) {
            OledClear(OLED_COLOR_BLACK);
            OledDrawString(greetingMessage);
            OledUpdate();
            turnCounter = 0;
            whosTurn = FIELD_OLED_TURN_NONE;
            message.type = MESSAGE_NONE;
            return message;
        } else if (event.type == BB_EVENT_START_BUTTON) {
            randomA = rand();
            hashA = NegotiationHash(randomA);
            FieldInit(&myField, &oppField);
            fieldSuccess = FieldAIPlaceAllBoats(&myField);
            sprintf(oledString, "CHALLENGING\nNumber: %d\nHash: %d", randomA, hashA);
            OledClear(OLED_COLOR_BLACK);
            OledDrawString(oledString);
            OledUpdate();
            if (fieldSuccess == SUCCESS) {
                message.type = MESSAGE_CHA;
                message.param0 = hashA;
                agentState = AGENT_STATE_CHALLENGING;
                return message;
            } else {
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("Error when placing boats");
                OledUpdate();
                agentState = AGENT_STATE_END_SCREEN;
                message.type = MESSAGE_NONE;
                return message;
            }
        } else if (event.type == BB_EVENT_CHA_RECEIVED) {
            randomB = rand();
            message.type = MESSAGE_ACC;
            message.param0 = randomB;
            FieldInit(&myField, &oppField);
            fieldSuccess = FieldAIPlaceAllBoats(&myField);
            if (fieldSuccess == SUCCESS) {
                agentState = AGENT_STATE_ACCEPTING;
                return message;
            } else {
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("Error when placing boats");
                OledUpdate();
                message.type = MESSAGE_NONE;
                return message;
            }           
        }
        break;
    case AGENT_STATE_CHALLENGING:
        if (event.type == BB_EVENT_RESET_BUTTON) {
            OledClear(OLED_COLOR_BLACK);
            OledDrawString(greetingMessage);
            OledUpdate();
            turnCounter = 0;
            whosTurn = FIELD_OLED_TURN_NONE;
            agentState = AGENT_STATE_START;
            message.type = MESSAGE_NONE;
            return message;
        } else if (event.type == BB_EVENT_ACC_RECEIVED) {
            message.type = MESSAGE_REV;
            message.param0 = randomA;
            coinFlip = NegotiateCoinFlip(randomA, randomB);
            if (coinFlip == HEADS) {
                agentState = AGENT_STATE_WAITING_TO_SEND;
                whosTurn = FIELD_OLED_TURN_MINE;
                FieldOledDrawScreen(&myField, &oppField, whosTurn, turnCounter);
                return message;
            } else if (coinFlip == TAILS) {
                agentState = AGENT_STATE_DEFENDING;
                whosTurn = FIELD_OLED_TURN_THEIRS;
                FieldOledDrawScreen(&myField, &oppField, whosTurn, turnCounter);
                return message;
            }          
        } else {
            message.type = MESSAGE_NONE;
            return message;
        }
        break;
    case AGENT_STATE_DEFENDING:
        if (event.type == BB_EVENT_RESET_BUTTON) {
            OledClear(OLED_COLOR_BLACK);
            OledDrawString(greetingMessage);
            OledUpdate();
            turnCounter = 0;
            whosTurn = FIELD_OLED_TURN_NONE;
            agentState = AGENT_STATE_START;
            message.type = MESSAGE_NONE;
            return message;
        } else if (event.type == BB_EVENT_SHO_RECEIVED) {
            oppGuess.col = event.param1;
            oppGuess.row = event.param0;
            FieldRegisterEnemyAttack(&myField, &oppGuess);
            FieldOledDrawScreen(&myField, &oppField, whosTurn, turnCounter);
            message.type = MESSAGE_RES;
            message.param0 = oppGuess.row;
            message.param1 = oppGuess.col;
            message.param2 = oppGuess.result;
            if (FieldGetBoatStates(&myField) == 0) {
                agentState = AGENT_STATE_END_SCREEN;
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("You Lost!");
                OledUpdate();
                message.type = MESSAGE_NONE;
                return message;
            } else {
                whosTurn = ChangeTurn(whosTurn);
                agentState = AGENT_STATE_WAITING_TO_SEND;
                return message;
            }
            return message;
        }
        break;
    case AGENT_STATE_WAITING_TO_SEND:
        if (event.type == BB_EVENT_RESET_BUTTON) {
            OledClear(OLED_COLOR_BLACK);
            OledDrawString(greetingMessage);
            OledUpdate();
            turnCounter = 0;
            whosTurn = FIELD_OLED_TURN_NONE;
            agentState = AGENT_STATE_START;
            message.type = MESSAGE_NONE;
            return message;
        } else if (event.type == BB_EVENT_MESSAGE_SENT) {
            turnCounter += 1;
            guessResult = FieldAIDecideGuess(&oppField);
            message.type = MESSAGE_SHO;
            message.param0 = guessResult.row;
            message.param1 = guessResult.col;
            agentState = AGENT_STATE_ATTACKING;
            return message;
        }
        break;
    case AGENT_STATE_ATTACKING:
        if (event.type == BB_EVENT_RESET_BUTTON) {
            OledClear(OLED_COLOR_BLACK);
            OledDrawString(greetingMessage);
            OledUpdate();
            turnCounter = 0;
            whosTurn = FIELD_OLED_TURN_NONE;
            agentState = AGENT_STATE_START;
            message.type = MESSAGE_NONE;
            return message;
        } else if (event.type == BB_EVENT_RES_RECEIVED) {
            guessResult.col = event.param1;
            guessResult.row = event.param0;
            guessResult.result = event.param2;
            FieldUpdateKnowledge(&oppField, &guessResult);
            FieldOledDrawScreen(&myField, &oppField, whosTurn, turnCounter);
            if (FieldGetBoatStates(&oppField) == 0) {
                agentState = AGENT_STATE_END_SCREEN;
                whosTurn = FIELD_OLED_TURN_NONE;
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("You won!");
                OledUpdate();
            } else {
                whosTurn = ChangeTurn(whosTurn);
                agentState = AGENT_STATE_DEFENDING;
            }
            message.type = MESSAGE_NONE;
            return message;
        }
        break;
    case AGENT_STATE_END_SCREEN:
        if (event.type == BB_EVENT_RESET_BUTTON) {
            OledClear(OLED_COLOR_BLACK);
            whosTurn = FIELD_OLED_TURN_NONE;
            OledDrawString(greetingMessage);
            OledUpdate();
            turnCounter = 0;
            agentState = AGENT_STATE_START;
            message.type = MESSAGE_NONE;
            return message;
        }
        break;
    case AGENT_STATE_ACCEPTING:
        if (event.type == BB_EVENT_RESET_BUTTON) {
            OledClear(OLED_COLOR_BLACK);
            OledDrawString(greetingMessage);
            OledUpdate();
            turnCounter = 0;
            whosTurn = FIELD_OLED_TURN_NONE;
            agentState = AGENT_STATE_START;
            message.type = MESSAGE_NONE;
            return message;
        } else if (event.type == BB_EVENT_REV_RECEIVED) {
            coinFlip = NegotiateCoinFlip(randomA, randomB);
            if (NegotiationVerify(randomA, hashA) == FALSE) {
                agentState = AGENT_STATE_END_SCREEN;
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("Cheating detected");
                OledUpdate();
                message.type = MESSAGE_NONE;
                return message;
            } else if (coinFlip == HEADS) {
                agentState = AGENT_STATE_DEFENDING;
                whosTurn = FIELD_OLED_TURN_THEIRS;
                FieldOledDrawScreen(&myField, &oppField, whosTurn, turnCounter);
                message.type = MESSAGE_NONE;
                return message;
            } else if (coinFlip == TAILS) {
                guessResult = FieldAIDecideGuess(&oppField);
                agentState = AGENT_STATE_ATTACKING;
                whosTurn = FIELD_OLED_TURN_MINE;
                FieldOledDrawScreen(&myField, &oppField, whosTurn, turnCounter);
                message.type = MESSAGE_SHO;
                message.param0 = guessResult.row;
                message.param1 = guessResult.col;
                return message;
            }
        }
        break;
    }
    message.type = MESSAGE_NONE;
    return message;
}

AgentState AgentGetState(void)
{
    return agentState;
}

void AgentSetState(AgentState newState)
{
    agentState = newState;
}

FieldOledTurn ChangeTurn(FieldOledTurn whosTurn)
{
    if (whosTurn == FIELD_OLED_TURN_MINE) {
        whosTurn = FIELD_OLED_TURN_THEIRS;
    } else if (FIELD_OLED_TURN_THEIRS) {
        whosTurn = FIELD_OLED_TURN_MINE;
    }
    return whosTurn;

}

