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

StudentWorld* Actor::getWorld()
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


////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

// Deterrent Base Class Implementation  (Actor -> Deterrent)

/**************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////////

int Deterrent::howMuchFoodHere()
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//Deterrent Implementations
////////////////////////////////////////////////////////////////////////////////////////////////////

    // Pebble
///////////////////////////////////////////////////////////////////////////////////////////////////////

void Pebble::doSomething()
{
    
    return;
}

    // Water Pool
///////////////////////////////////////////////////////////////////////////////////////////////////////



    // Poison
///////////////////////////////////////////////////////////////////////////////////////////////////////




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
bool EnergyHolder::myFoodEaten(int& howMuch, int unitsToConsume)
{
    if(isFood)
    {
        decreaseEnergyBy(getWorld()->consumableFood(getX(), getY(), unitsToConsume));
        howMuch = getWorld()->consumableFood(getX(), getY(), unitsToConsume);
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//Energy Holder Implementations
////////////////////////////////////////////////////////////////////////////////////////////////////

// Anthill
///////////////////////////////////////////////////////////////////////////////////////////////////////

void Anthill::doSomething()
{
    return; //change later
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

int Anthill::howMuchFoodHere()
{
    return 0;
}


// Food
///////////////////////////////////////////////////////////////////////////////////////////////////////
void Food::doSomething()
{
    return;
}

int Food::howMuchFoodHere()
{
    return 0;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

// Insect Base Class Implementation (Actor -> EnergyHolder -> Insect)

/**************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////////

bool Insect::checkIfStunnedOrSleeping()
{
    if(m_sleepTicks > 0 || isStunned)
        return true;
    return false;
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
    
    int howMuch = 0;
    if(myFoodEaten(howMuch, unitsToConsume)) //pass in a locally created variable by reference to see how much was eaten
    {
        increaseEnergyBy(howMuch);
        return true;
    }
    return false;
}

void Insect::becomeFood()   //when any insect dies, it becomes food
{
    getWorld()->addFoodToSquare(getX(), getY());
}

void Insect::resetSleepTicks()
{
    m_sleepTicks = 2;
}

int Insect::howMuchFoodHere()
{
    return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Ant Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////


//interpreter
bool Ant::instructionInterpreter()
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Grasshopper Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////


void GrassHopper::doSomething()
{
    return;
}

int GrassHopper::randomDistance()
{
    return (rand() % 9) + 2;
}

void GrassHopper::resetDistanceToWalk(int d)
{
    distanceToWalk = d;
}
int GrassHopper::getDistanceToWalk()
{
    return distanceToWalk;
}
void GrassHopper::walk(Direction curr)
{
    switch(curr)
    {
        case up:
            moveTo(getX(), getY()-1);
            break;
        case down:
            moveTo(getX(), getY()+1);
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
    distanceToWalk--;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Baby Grasshopper Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////


void BabyGrassHopper::doSomething()
{
    decreaseEnergyBy(1);
    
    if(getEnergyUnits()+getWorld()->getCurrentTicks() != 500){
        ;
    }
    
    if(getEnergyUnits() == 0)
    {
        becomeFood();
        setDead();
        return;
    }
    if(checkIfStunnedOrSleeping())
    {
        decreaseSleepTicks();
        return;
    }
    if(getEnergyUnits() >= 1600)
    {
        //turn into an adult grasshopper
        becomeFood();
        setDead();
        return;
    }
    
    //if(eatFood(true))
    //{
    if(rand()%2 == 0){
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
        }
    }
    //}
    resetSleepTicks();
}

////////////////////////////////////////////////////////////////////////////////////////////////////


