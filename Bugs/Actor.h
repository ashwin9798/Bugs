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

    virtual int howMuchFoodHere();
    virtual int howManyPheromonesHere();
    virtual bool isDangerousToAnt(int colonyNumber);   //virtual function to identify dangerous objects to an ant of a specified colony number
    
private:
    StudentWorld* myWorld;
    bool m_alive;
    bool hasActedDuringTick;
};

class EnergyHolder : public Actor       //abstract class for all objects that have hit points
{
public:
    EnergyHolder(StudentWorld* ptr, int imgID, int x, int y, int initialHitPoints, bool isFood = false, Direction startDirection = none, int depth = 0): Actor(ptr,imgID, x,y,startDirection, depth), energyUnits(initialHitPoints){}
    
    ~EnergyHolder(){}
    
    virtual void doSomething()=0;
    int getEnergyUnits() const;
    void increaseEnergyBy(int units);
    void decreaseEnergyBy(int units);
    
    int myFoodEaten(int unitsToConsume);
    
private:
    int energyUnits;
};



class Deterrent: public Actor      //Poison, and water pools
{
public:
    Deterrent(StudentWorld* ptr, int imgID, int x, int y, Direction start, int depth): Actor(ptr, imgID, x,y,start,depth){}
    ~Deterrent(){}
    
    virtual void doSomething() = 0;
    virtual void harmInsect(bool isStunning);  //Stunning and poisoning are the same process, (one calls setBitten, the other calls setPoison)
};





class Pebble : public Actor     //Pebble is unique and is not derived from any class
{
public:
    Pebble(StudentWorld* ptr, int x, int y): Actor(ptr,IID_ROCK,x,y,right,1){}
    virtual void doSomething();
};




class Insect : public EnergyHolder     //all insects have hit points
{
public:
    Insect(StudentWorld* ptr, int imgID, int x, int y,  int initialHitPoints, Direction startDirection = none, int depth = 0): EnergyHolder(ptr, imgID, x, y, initialHitPoints, false, startDirection, depth), isStunned(false), isBitten(false), m_sleepTicks(0){};
    virtual void doSomething() = 0;
    ~Insect(){}
    
    virtual void setPoisoned();
    virtual void setBitten(int damage)=0;
    
    virtual void setStunnedState(bool state);
    
    bool checkIfSleeping();
    bool checkIfStunned();
    
    void decreaseSleepTicks();
    void resetSleepTicks();
    
    bool eatFood(bool isGrasshopper);
    void becomeFood();

    bool isInsectDead();
    void walk(Direction curr);    //decrements by 1 and moves the grasshopper
    
private:
    bool isStunned;
    bool isBitten;
    int m_sleepTicks;
};


//Insects

class Ant : public Insect
{
public:
    //colony number is the anthill index (compiler index)
    Ant(StudentWorld* ptr, Compiler* whichCompiler, int imageID, int startX, int startY):
    Insect(ptr, imageID, startX, startY,  1500, pickRandomDirection(), 1),
    m_pointerToMyCompilerObject(whichCompiler),
    instructionCounter(0)
    {
        colonyNumber = imageID;         //colony number is the same as the constants used for image ID
        wasBitten = false;
        wasBlocked = false;
        storedFood = 0;
        birthX = startX;
        birthY = startY;
    }
    virtual void doSomething();
    virtual void setBitten(int damage);
    bool interpretInstructions();
    int getColonyNumber();
    bool isDangerousToAnt(int colonyNumber);
    
private:
    Compiler* m_pointerToMyCompilerObject;
    int instructionCounter;
    int colonyNumber;
    bool wasBitten;
    bool wasBlocked;
    int lastRandomNumberGenerated;
    int storedFood;
    int birthX;
    int birthY;
};


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
    void decreaseDistanceToWalk();
    bool isDangerousToAnt(int colonyNumber);
    void jumpRandomly();
    
private:
    int distanceToWalk;
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


//Other Energy Holders

class Anthill : public EnergyHolder
{
public:
    //Constructor
    Anthill(StudentWorld* ptr, Compiler* whichCompiler, int imageIDForAnts, int xPos, int yPos):
    EnergyHolder(ptr, IID_ANT_HILL, xPos, yPos, 8999, false, right, 2),
    m_compiler(whichCompiler),
    m_numberOfAnts(5),
    myImageID(imageIDForAnts)
    {
        m_colonyName = m_compiler->getColonyName();
    }
    virtual void doSomething();
    
    //Accessors
    int getNumberOfAnts();
    std::string getColonyName();
    
    //Mutators
    void setCompiler(Compiler* c);
    
private:
    std::string m_colonyName;
    int m_numberOfAnts;
    Compiler* m_compiler;
    int myImageID;
};

class Food : public EnergyHolder
{
public:
    Food(StudentWorld* ptr, int x, int y, int startUnits = 6000): EnergyHolder(ptr, IID_FOOD, x, y, startUnits, true, right, 2){}
    virtual void doSomething();
    virtual int howMuchFoodHere();
};

class Pheromone : public EnergyHolder
{
public:
    Pheromone(StudentWorld* ptr, int imageID, int x, int y, int startUnits = 256): EnergyHolder(ptr, imageID, x, y, startUnits, false, right, 2), m_colonyNumber(imageID){}
    virtual void doSomething();
    virtual int howManyPheromonesHere();
    int getColonyNumber();
private:
    int m_colonyNumber;
};

//Deterrents

class WaterPool : public Deterrent
{
public:
    WaterPool(StudentWorld* ptr, int x, int y): Deterrent(ptr,IID_WATER_POOL, x, y, right, 2){}
    virtual void doSomething();
};

class Poison : public Deterrent
{
public:
    Poison(StudentWorld* ptr, int x, int y): Deterrent(ptr, IID_POISON, x, y, right, 2){}
    virtual void doSomething();
    virtual bool isDangerousToAnt(int colonyNumber);
};


#endif /* Actor_h */
