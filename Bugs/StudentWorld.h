#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <list>
#include <vector>
#include <string>

using namespace std;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);

    virtual int init();

    virtual int move();

    virtual void cleanUp();

    void displayGameText();
    std::string formatGameText(int ticks, int antsAnt0, int antsAnt1, int antsAnt2, int antsAnt3, int winningAntNumber);
    
    int getCurrentTicks() const;
    void updateTicks();
    int getWinningAntNumber();
    bool checkForCompilerError(Compiler* c);
    bool loadFieldFile();
    
    void addObjectToSimulation(Actor* object, int x, int y);        //add to m_container
    void removeObjectFromSimulation(Actor* object, int x, int y);
    void changeObjectPosition(Actor* object, int originalX, int originalY, int x, int y);
    
    bool doesBlockInsect(Actor* a, int x, int y, Actor::Direction curr);
    int checkForFood(int x, int y);   //returns how much food is in the square
    
    Actor* getFoodObject(int x, int y);
    void addFoodToSquare(int x, int y);

    
private:
    int m_ticks;
    int m_numberOfAnthills;

    list<Actor*> m_container[64][64];
};

#endif // STUDENTWORLD_H_
