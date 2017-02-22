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


    // Water Pool
///////////////////////////////////////////////////////////////////////////////////////////////////////

void WaterPool::doSomething()
{
    harmInsect();
}

void WaterPool::harmInsect()
{
    getWorld()->stunInsect(getX(), getY());
}

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
    if(getWorld()->totalFood(getX(), getY())>0)
    {
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
    
    int howMuch = 0;
    if(myFoodEaten(howMuch, unitsToConsume)) //pass in a locally created variable by reference to see how much was eaten
    {
        increaseEnergyBy(howMuch);
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

int Insect::howMuchFoodHere()
{
    return 0;
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
void Insect::setStunned()
{
    if(isStunned)
        isStunned = false;
    else{
        isStunned = true;
        m_sleepTicks+=2;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Ant Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////


//interpreter
bool Ant::instructionInterpreter()
{
    return true;
}

void Ant::setBitten(int damage)
{
    return;
    //ADD MORE
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
        getWorld()->bite(50, getX(), getY(), dynamic_cast<Insect*>(this));
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
void GrassHopper::walk(Direction curr)
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
    distanceToWalk--;
}

void GrassHopper::setBitten(int damage)
{
    decreaseEnergyBy(damage);
    if(isInsectDead())
        return;
    if(rand()%2 == 1)       //50% chance
    {
        getWorld()->bite(50, getX(), getY(), dynamic_cast<Insect*>(this));
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
            setStunned();       //when the insect moves, it is not stunned
        walk(current);
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

int Pebble::howMuchFoodHere()
{
    return 0;
}
