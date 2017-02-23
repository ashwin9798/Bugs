//
//  Actor.cpp
//  CS 32 Project 3
//
//  Created by Ashwin Vivek on 2/12/17.
//  Copyright © 2017 AshwinVivek. All rights reserved.
//

//#include "Compiler.h"
#include "Actor.h"
#include "GraphObject.h"
#include "StudentWorld.h"
#include <random>
#include "Compiler.h"
#include <cmath>
#include <string>
const double PI = 4*atan(1.0);

////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

// Actor Base Class Implementation

/**************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////////

//Random start direction
Actor::Direction Actor::pickRandomDirection()
{
    Direction start = none;
    
    int randomDirection = (rand() % 4);
    
    switch(randomDirection)
    {
        case 0:
            start = up;
            return start;
        case 1:
            start = down;
            return start;
        case 2:
            start = left;
            return start;
        case 3:
            start = right;
            return start;
    }
    return start;
}

StudentWorld* Actor::getWorld() const
{
    return myWorld;
}
void Actor::setDead()       //after each tick, check which actors are dead
{
    m_alive = false;
}
bool Actor::isAlive()
{
    return m_alive;
}
bool Actor::hasDoneSomethingThisTick()
{
    return hasActedDuringTick;
}
void Actor::setActed(bool afterTick)
{
    if(afterTick)
        hasActedDuringTick = true;
    else
        hasActedDuringTick = false;
}
int Actor::howManyPheromonesHere()
{
    return 0;
}
int Actor::howMuchFoodHere()
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

// Deterrent Base Class Implementation  (Actor -> Deterrent)

/**************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////////

void Deterrent::harmInsect(bool isStunning)
{
    getWorld()->harmInsect(getX(), getY(), isStunning);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//Deterrent Implementations
////////////////////////////////////////////////////////////////////////////////////////////////////


    // Water Pool
///////////////////////////////////////////////////////////////////////////////////////////////////////

void WaterPool::doSomething()
{
    harmInsect(true);
}


    // Poison
///////////////////////////////////////////////////////////////////////////////////////////////////////

void Poison::doSomething()
{
    harmInsect(false);      //false identifies it as poison
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

// Energy Holder Base Class Implementation (Actor -> EnergyHolder)

/**************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////////

int EnergyHolder::getEnergyUnits() const
{
    return energyUnits;
}
void EnergyHolder::increaseEnergyBy(int units)
{
    energyUnits += units;
}
void EnergyHolder::decreaseEnergyBy(int units)
{
    energyUnits -= units;
}
int EnergyHolder::myFoodEaten(int unitsToConsume) //returns how much food is to be eaten to the caller, and also decrements the energy units of the food at that position
{
    int howMuch = 0;
    if(getWorld()->totalFood(getX(), getY())>0)
    {
        howMuch = getWorld()->consumableFood(getX(), getY(), unitsToConsume);
        return howMuch;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//Energy Holder Implementations
////////////////////////////////////////////////////////////////////////////////////////////////////

// Anthill
///////////////////////////////////////////////////////////////////////////////////////////////////////

void Anthill::doSomething()
{
    decreaseEnergyBy(1);
    if(getEnergyUnits()<=0){
        setDead();
        return;
    }
    if(myFoodEaten(10000)>0){
        increaseEnergyBy(myFoodEaten(10000));
        return;
    }
    if(getEnergyUnits()>=2000){
        getWorld()->giveBirthToAnt(getX(), getY(), m_compiler, myImageID);
        decreaseEnergyBy(1500);
        m_numberOfAnts++;
    }
    
}

//Accessors
int Anthill::getNumberOfAnts()
{
    return m_numberOfAnts;
}

std::string Anthill::getColonyName()
{
    return m_colonyName;
}

//Mutators

void Anthill::setCompiler(Compiler* c)
{
    m_compiler = c;
}


// Food
///////////////////////////////////////////////////////////////////////////////////////////////////////
void Food::doSomething()
{
    return;
}

int Food::howMuchFoodHere()
{
    return getEnergyUnits();
}


// Pheromone
///////////////////////////////////////////////////////////////////////////////////////////////////////

void Pheromone::doSomething()
{
    decreaseEnergyBy(1);
    if(getEnergyUnits()==0)
        setDead();
}

int Pheromone::getColonyNumber()
{
    return m_colonyNumber;
}

int Pheromone::howManyPheromonesHere()
{
    return getEnergyUnits();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

// Insect Base Class Implementation (Actor -> EnergyHolder -> Insect)

/**************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////////

bool Insect::checkIfSleeping()
{
    if(m_sleepTicks > 0)
        return true;
    return false;
}

bool Insect::checkIfStunned()
{
    return isStunned;
}

void Insect::decreaseSleepTicks()
{
    m_sleepTicks--;
}
bool Insect::eatFood(bool isGrasshopper)    //returns true if food was eaten, otherwise false
{
    int unitsToConsume;
    if(isGrasshopper)
        unitsToConsume = 200;
    else
        unitsToConsume = 100;
    
    if(myFoodEaten(unitsToConsume) > 0)
    {
        increaseEnergyBy(myFoodEaten(unitsToConsume));
        return true;
    }
    return false;
}

void Insect::becomeFood()   //when any insect dies, it becomes 100 units of food in that position
{
    getWorld()->addFoodToSquare(getX(), getY());
}

void Insect::resetSleepTicks()
{
    m_sleepTicks = 2;
}

bool Insect::isInsectDead()
{
    if(getEnergyUnits() <= 0)
    {
        becomeFood();
        setDead();
        return true;
    }
    return false;
}

void Insect::setStunnedState(bool state)
{
    if(state)
        m_sleepTicks+=2;
    isStunned = state;
}

void Insect::setPoisoned()
{
    decreaseEnergyBy(150);
    if(isInsectDead())
        return;
    //interpret commands
}

void Insect::walk(Direction curr)
{
    switch(curr)
    {
        case up:
            moveTo(getX(), getY()+1);
            break;
        case down:
            moveTo(getX(), getY()-1);
            break;
        case left:
            moveTo(getX()-1, getY());
            break;
        case right:
            moveTo(getX()+1, getY());
            break;
        case none:
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Ant Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////
void Ant::doSomething()
{
    decreaseEnergyBy(1);
    if(isInsectDead())
        return;
    if(checkIfStunned())
    {
        decreaseSleepTicks();
        if(checkIfSleeping())
            setStunnedState(false);
    }
}

//interpreter
bool Ant::interpretInstructions()
{
    Compiler::Command cmd;
    bool doneInterpreting = false;
    
    for(;;)
    {
        if(!m_pointerToMyCompilerObject->getCommand(instructionCounter, cmd)){
            return false;
        }
        switch(cmd.opcode)
        {
            case Compiler::moveForward:
                if(getWorld()->hasPebbleAt(getX(), getY(), getDirection()))
                    wasBlocked = true;
                else{
                    walk(getDirection());
                    wasBlocked=false;
                    wasBitten=false;
                }
                doneInterpreting = true;
                break;
            case Compiler::eatFood:
                if(storedFood >= 100)
                    increaseEnergyBy(100);
                else if(storedFood > 0)
                    increaseEnergyBy(storedFood);
                
                break;
            case Compiler::dropFood:
                getWorld()->addFoodToSquare(getX(), getY());
                break;
            case Compiler::bite:
                getWorld()->bite(15, getX(), getY(), static_cast<Insect*>(this));
                break;
            case Compiler::pickupFood:
                int pickUp;
                if(storedFood <= 1400)
                    pickUp = getWorld()->consumableFood(getX(), getY(), 400);
                else
                    pickUp = getWorld()->consumableFood(getX(), getY(), 1800-storedFood);
                storedFood += pickUp;
                break;
            case Compiler::emitPheromone:
                getWorld()->emitPheromone(getX(),getY(), colonyNumber+11, colonyNumber);
                break;
            case Compiler::faceRandomDirection:
                setDirection(pickRandomDirection());
                break;
            case Compiler::generateRandomNumber:
                break;
            case Compiler::if_command:
                if(cmd.operand1 == "last_random_number_was_zero")
                    
            
                
//            case invalid:
//                break;
//            case label,
//                goto_command,
//                if_command,
//                emitPheromone,
//                faceRandomDirection,
//                rotateClockwise,
//                rotateCounterClockwise,

//                bite,
//                pickupFood,
//                dropFood,
//                eatFood,
//                generateRandomNumber
        }
    }
}

void Ant::setBitten(int damage)
{
    return;
    //ADD MORE
}

int Ant::getColonyNumber()
{
    return colonyNumber;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// Grasshopper Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////


void GrassHopper::doSomething()
{
    decreaseEnergyBy(1);
    
    if(isInsectDead())
        return;
    if(checkIfSleeping()){
        decreaseSleepTicks();
        return;
    }
    if(randInt(1, 3)==1)
        getWorld()->bite(50, getX(), getY(), static_cast<Insect*>(this));
    else
    {
        if(randInt(1,10) == 1)          //1 in 10 chance
        {
            jumpRandomly();
            resetSleepTicks();
            return;
        }
        if(eatFood(true))
            if(rand()%2 == 0)
            {
                resetSleepTicks();
                return;
            }
        if(getDistanceToWalk()==0)
        {
            setDirection(pickRandomDirection());
            resetDistanceToWalk(randomDistance());
        }
        Direction current = getDirection();
        if(getWorld()->hasPebbleAt(getX(), getY(), current))
        {
            resetDistanceToWalk(0);
        }
        else
        {
            walk(current);
            decreaseDistanceToWalk();
        }
        resetSleepTicks();
    }
    
}
int GrassHopper::randomDistance()
{
    return (rand() % 9) + 2;
}

void GrassHopper::resetDistanceToWalk(int d)
{
    distanceToWalk = d;
}
int GrassHopper::getDistanceToWalk() const
{
    return distanceToWalk;
}

void GrassHopper::setBitten(int damage)
{
    decreaseEnergyBy(damage);
    if(isInsectDead())
        return;
    if(rand()%2 == 1)       //50% chance
    {
        getWorld()->bite(50, getX(), getY(), this);
    }
}

void GrassHopper::jumpRandomly()
{
    int destX;
    int destY;
    
    double angle = 2*((double)rand()/double(RAND_MAX));
    int distance = randInt(0, 10);
    
    destX = getX()+(int)distance*cos(angle*PI);
    destY = getY()+(int)distance*sin(angle*PI);
    
    if((((destX<0 || destX>= VIEW_WIDTH-1) || (destY<0)||destY>=VIEW_HEIGHT-1)) || getWorld()->hasPebbleAt(destX, destY, none))
        jumpRandomly();
    else{
        moveTo(destX, destY);
        return;
    }
}

void GrassHopper::decreaseDistanceToWalk()
{
    distanceToWalk--;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Baby Grasshopper Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////


void BabyGrassHopper::doSomething()
{
    decreaseEnergyBy(1);

    if(isInsectDead())
        return;
    
    if(checkIfSleeping()){
        decreaseSleepTicks();
        return;
    }
    if(getEnergyUnits() >= 1600)
    {
        becomeAdult();
        becomeFood();
        setDead();
        return;
    }
    
    if(eatFood(true))
        if(rand()%2 == 0)
        {
            resetSleepTicks();
            return;
        }
    
    if(getDistanceToWalk()==0)
    {
        setDirection(pickRandomDirection());
        resetDistanceToWalk(randomDistance());
    }
    Direction current = getDirection();
    if(getWorld()->hasPebbleAt(getX(), getY(), current))
    {
        resetDistanceToWalk(0);
    }
    else
    {
        if(checkIfStunned())
            setStunnedState(false);       //when the insect moves, it is not stunned
        walk(current);
        decreaseDistanceToWalk();
    }
    
    resetSleepTicks();
}

void BabyGrassHopper::becomeAdult()
{
    getWorld()->becomeAdultGrassHopper(getX(), getY());
}

void BabyGrassHopper::setBitten(int damage)
{
    decreaseEnergyBy(damage);
    if(isInsectDead())
        return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////

//Pebble Implementation

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Pebble::doSomething()
{
    return;
}
