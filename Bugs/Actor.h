//
//  Actor.h
//  CS 32 Project 3
//
//  Created by Ashwin Vivek on 2/12/17.
//  Copyright © 2017 AshwinVivek. All rights reserved.
//

#ifndef Actor_h
#define Actor_h

#include "GraphObject.h"
#include <string>
#include "Compiler.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* ptr, int imgID, int x, int y, Direction startDirection, int depth):
        GraphObject(imgID, x, y, startDirection, depth),
        m_alive(true), myWorld(ptr), hasActedDuringTick(true)
        {
            setVisible(true);
        }
    
    ~Actor(){}

    virtual void doSomething() = 0;
    Direction pickRandomDirection();
    StudentWorld* getWorld() const;
    void setDead();
    bool isAlive();
    
    bool hasDoneSomethingThisTick();        //each actor can only act once even if position changed
    void setActed(bool afterTick);     //sets hasActedDuringTick to true after each tick, and false before each tick

    virtual int howMuchFoodHere()=0;
    
private:
    StudentWorld* myWorld;
    bool m_alive;
    bool hasActedDuringTick;
};

class EnergyHolder : public Actor       //abstract class for all objects that have hit points
{
public:
    EnergyHolder(StudentWorld* ptr, int imgID, int x, int y, int initialHitPoints, bool isFood = false, Direction startDirection = none, int depth = 0): Actor(ptr,imgID, x,y,startDirection, depth), energyUnits(initialHitPoints), isFood(isFood){}
    
    ~EnergyHolder(){}
    
    virtual void doSomething()=0;
    int getEnergyUnits() const;
    void increaseEnergyBy(int units);
    void decreaseEnergyBy(int units);
    
    bool myFoodEaten(int& howMuch, int unitsToConsume);
    
    virtual int howMuchFoodHere()=0;
    
private:
    int energyUnits;
    bool isFood;
};

class Deterrent: public Actor      //Pebbles, poison, and water pools
{
public:
    Deterrent(StudentWorld* ptr, int imgID, int x, int y, Direction start, int depth): Actor(ptr, imgID, x,y,start,depth){}
    ~Deterrent(){}
    
    virtual void doSomething() = 0;

    virtual int howMuchFoodHere();
};

class Insect : public EnergyHolder     //all insects have hit points
{
public:
    Insect(StudentWorld* ptr, int imgID, int x, int y,  int initialHitPoints, Direction startDirection = none, int depth = 0): EnergyHolder(ptr, imgID, x, y, initialHitPoints, false, startDirection, depth), isStunned(false), isBitten(false), m_sleepTicks(0){};
    virtual void doSomething() = 0;
    ~Insect(){}
//    virtual void setPoisoned();
    virtual void setBitten(int damage)=0;
    virtual int howMuchFoodHere();
    
    bool checkIfStunnedOrSleeping();
    void decreaseSleepTicks();
    void resetSleepTicks();
    
    bool eatFood(bool isGrasshopper);
    void becomeFood();

    bool isInsectDead();
    
private:
    bool isStunned;
    bool isBitten;
    int m_sleepTicks;
};

//Insects

class GrassHopper : public Insect
{
public:
    GrassHopper(StudentWorld* ptr, int startX, int startY, int imageID = IID_ADULT_GRASSHOPPER, int initialHitPoints = 1600):
    Insect(ptr, imageID, startX, startY, initialHitPoints, pickRandomDirection(), 1)
    {
        distanceToWalk = randomDistance();
    }
    virtual void doSomething();
    virtual void setBitten(int damage);
    int randomDistance();
    
    int getDistanceToWalk() const;
    void resetDistanceToWalk(int d);
    
    void walk(Direction curr);    //decrements by 1 and moves the grasshopper
    void jumpRandomly();
    
private:
    int distanceToWalk;
};

class Ant : public Insect
{
public:
    //colony number is the anthill index (compiler index)
    Ant(StudentWorld* ptr, Compiler* whichCompiler, int imageID, int startX, int startY):
    Insect(ptr, imageID, startX, startY,  1500, pickRandomDirection(), 1),
    m_pointerToMyCompilerObject(whichCompiler),
    instructionCounter(0){}
    virtual void doSomething();
    virtual void setBitten(int damage);
    bool instructionInterpreter();
    
private:
    Compiler* m_pointerToMyCompilerObject;
    //food
//    bool wasBlocked;
    int instructionCounter;
};

class BabyGrassHopper : public GrassHopper
{
public:
    BabyGrassHopper(StudentWorld* ptr, int startX, int startY): GrassHopper(ptr, startX, startY, IID_BABY_GRASSHOPPER,500){}
    virtual void doSomething();
    virtual void setBitten(int damage);
    void becomeAdult();
private:

};

//Energy Holders

class Anthill : public EnergyHolder
{
public:
    //Constructor
    Anthill(StudentWorld* ptr, Compiler* whichCompiler, int colonyNumber, int xPos, int yPos):
    EnergyHolder(ptr, IID_ANT_HILL, xPos, yPos, 8999, false, right, 2),
    m_compiler(whichCompiler),
    m_numberOfAnts(5)
    {
        m_colonyName = m_compiler->getColonyName();
    }
    virtual void doSomething();
    virtual int howMuchFoodHere();
    //Accessors
    int getNumberOfAnts();
    std::string getColonyName();
    
    //Mutators
    void setCompiler(Compiler* c);
    
private:
    std::string m_colonyName;
    int m_numberOfAnts;
    Compiler* m_compiler;
};

class Food : public EnergyHolder
{
public:
    Food(StudentWorld* ptr, int x, int y, int startUnits = 6000): EnergyHolder(ptr, IID_FOOD, x, y, startUnits, true, right, 2){}
    virtual void doSomething();
    virtual int howMuchFoodHere();
};


//Deterrents

class Pebble : public Deterrent
{
public:
    Pebble(StudentWorld* ptr, int x, int y): Deterrent(ptr,IID_ROCK,x,y,right,1){}
    virtual void doSomething();
};



#endif /* Actor_h */
