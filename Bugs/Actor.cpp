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
// Actor Base Class Implementation
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

void Actor::addToWorld(StudentWorld *world)
{
    myWorld = world;
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


////////////////////////////////////////////////////////////////////////////////////////////////////
// Energy Holder Base Class Implementation
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
bool EnergyHolder::myFoodEaten(int& howMuch)
{
    if(isFood)
    {
        decreaseEnergyBy(getWorld()->checkForFood(getX(), getY()));
        howMuch = getWorld()->checkForFood(getX(), getY());
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Insect Base Class Implementation
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
bool Insect::eatFood()    //returns true if food was eaten, otherwise false
{
    int howMuch = 0;
    if(myFoodEaten(howMuch)) //pass in a locally created variable by reference to see how much was eaten
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

////////////////////////////////////////////////////////////////////////////////////////////////////
// Anthill Implementation
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
bool Anthill::doesBlockInsect()
{
    return false;
}

int Anthill::howMuchFoodHere()
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

bool Ant::doesBlockInsect()
{
    return false;
}

int Ant::howMuchFoodHere()
{
    return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Grasshopper Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////


void GrassHopper::doSomething()
{
    return;
}
bool GrassHopper::doesBlockInsect()
{
    return false;
}
int GrassHopper::randomDistance()
{
    return (rand() % 9) + 2;
}
int GrassHopper::howMuchFoodHere()
{
    return 0;
}
void GrassHopper::resetDistanceToWalk(int d)
{
    distanceToWalk = d;
}
int GrassHopper::getDistanceToWalk()
{
    return distanceToWalk;
}
void GrassHopper::walk()
{
    distanceToWalk--;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Baby Grasshopper Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////


void BabyGrassHopper::doSomething()
{
    decreaseEnergyBy(1);
    
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
    
    if(eatFood())
    {
        if(rand()%2 == 0){
            if(getDistanceToWalk()==0)
            {
                setDirection(pickRandomDirection());
                resetDistanceToWalk(randomDistance());
            }
            Direction current = getDirection();
            if(getWorld()->doesBlockInsect(this, getX(), getY(), current))
            {
                resetDistanceToWalk(0);
            }
            else
            {
                walk();
            }
        }
    }
    resetSleepTicks();
}
bool BabyGrassHopper::doesBlockInsect()
{
    return false;
}
int BabyGrassHopper::howMuchFoodHere()
{
    return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Pebble Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////

void Pebble::doSomething()
{
    return;
}
bool Pebble::doesBlockInsect()
{
    return true;
}
int Pebble::howMuchFoodHere()
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Food Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////

int Food::howMuchFoodHere()
{
    return 0;
}

