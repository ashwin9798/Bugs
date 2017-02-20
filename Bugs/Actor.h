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
    Actor(int imgID, int x, int y, Direction startDirection, int depth):
        GraphObject(imgID, x, y, startDirection, 1, depth),
        m_alive(true)
        {
            setVisible(true);
        }
    virtual void doSomething() = 0;
    Direction pickRandomDirection();
    void addToWorld(StudentWorld* world);
    StudentWorld* getWorld();
    void setDead();
    bool isAlive();
    
    virtual bool doesBlockInsect()=0;
    virtual int howMuchFoodHere()=0;
    
private:
    StudentWorld* myWorld;
    bool m_alive;
};

class EnergyHolder : public Actor       //abstract class for all objects that have hit points
{
public:
    EnergyHolder(int imgID, int x, int y, int initialHitPoints, bool isFood = false, Direction startDirection = none, int depth = 0): Actor(imgID, x,y,startDirection, depth), energyUnits(initialHitPoints){}
    virtual void doSomething()=0;
    int getEnergyUnits() const;
    void increaseEnergyBy(int units);
    void decreaseEnergyBy(int units);
    
    bool myFoodEaten(int& howMuch);
    
    virtual bool doesBlockInsect()=0;
    virtual int howMuchFoodHere()=0;
    
private:
    int energyUnits;
    bool isFood;
};

class Deterrent: public Actor      //Pebbles, poison, and water pools
{
public:
    Deterrent(int imgID, int x, int y, Direction start, int depth): Actor(imgID, x,y,start,depth){}
    
    virtual void doSomething() = 0;
    
    virtual bool doesBlockInsect()=0;
    virtual int howMuchFoodHere()=0;
};

class Insect : public EnergyHolder     //all insects have hit points
{
public:
    Insect(int imgID, int x, int y,  int initialHitPoints, Direction startDirection = none, int depth = 0): EnergyHolder(imgID, x, y, initialHitPoints, false, startDirection, depth), isStunned(false), isBitten(false), m_sleepTicks(0){};
    virtual void doSomething() = 0;
//    virtual void setPoisoned();
//    virtual void setBitten();
    virtual bool doesBlockInsect()=0;
    
    bool checkIfStunnedOrSleeping();
    void decreaseSleepTicks();
    void resetSleepTicks();
    
    bool eatFood();
    void becomeFood();
    
    virtual int howMuchFoodHere()=0;
    
private:
    bool isStunned;
    bool isBitten;
    int m_sleepTicks;
};

//Insects

class GrassHopper : public Insect
{
public:
    GrassHopper(int startX, int startY, int imageID, int initialHitPoints = 1600):
    Insect(imageID, startY, startY, initialHitPoints, pickRandomDirection(), 1)
    {
        distanceToWalk = randomDistance();
    }
    virtual void doSomething();
    int randomDistance();
    
    
    virtual bool doesBlockInsect();
    virtual int howMuchFoodHere();
    
    int getDistanceToWalk();
    void resetDistanceToWalk(int d);
    void walk();    //decrements by 1

private:
    int distanceToWalk;
};

class Ant : public Insect
{
public:
    //colony number is the anthill index (compiler index)
    Ant(Compiler* whichCompiler, int imageID, int startX, int startY):
    Insect(imageID, startX, startY,  1500, pickRandomDirection(), 1),
    m_pointerToMyCompilerObject(whichCompiler),
    instructionCounter(0){}
    bool instructionInterpreter();
    virtual bool doesBlockInsect();
    virtual int howMuchFoodHere();
    
private:
    Compiler* m_pointerToMyCompilerObject;
    //food
//    bool wasBlocked;
    int instructionCounter;
};

class BabyGrassHopper : public GrassHopper
{
public:
    BabyGrassHopper(int startX, int startY): GrassHopper(startX, startY, IID_BABY_GRASSHOPPER,500){}
    virtual void doSomething();
    virtual bool doesBlockInsect();
    virtual int howMuchFoodHere();
private:

};

//Energy Holders

class Anthill : public EnergyHolder
{
public:
    //Constructor
    Anthill(Compiler* whichCompiler, int colonyNumber, int xPos, int yPos):
    EnergyHolder(IID_ANT_HILL, xPos, yPos, 8999, false, right, 2),
    m_compiler(whichCompiler),
    m_numberOfAnts(5)
    {
        m_colonyName = m_compiler->getColonyName();
    }
    virtual void doSomething();
    virtual bool doesBlockInsect();
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
    Food(int x, int y, int startUnits = 6000): EnergyHolder(IID_FOOD, x, y, startUnits, true, right, 2){}
    virtual void doSomething();
    virtual bool doesBlockInsect();
    virtual int howMuchFoodHere();
};


//Deterrents

class Pebble : public Deterrent
{
public:
    Pebble(int x, int y): Deterrent(IID_ROCK,x,y,right,1){}
    virtual void doSomething();
    virtual bool doesBlockInsect();
    virtual int howMuchFoodHere();
};



#endif /* Actor_h */
