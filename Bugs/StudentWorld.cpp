//
//  StudentWorld.cpp
//  CS 32 Project 3
//
//  Created by Ashwin Vivek on 2/12/17.
//  Copyright © /Users/ashwinvivek/Documents/UCLA Documents/UCLA Classes/Quarter 2/CS 32/CS 32 Project 3/GraphObject.h2017 AshwinVivek. All rights reserved.
//

#include "StudentWorld.h"
#include "GameWorld.h"
#include "Field.h"
#include "Compiler.h"
#include "Actor.h"
#include <list>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

StudentWorld::StudentWorld(string assetDir): GameWorld(assetDir), m_ticks(0), m_numberOfAnthills(0)
{
    for(int i=0; i<64; i++){
        for(int j=0; j<64; j++)
        {
            m_container[i][j].front()=nullptr;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Pure Virtual Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////

int StudentWorld::init()
{
    m_ticks = 0;
    
    if(!loadFieldFile())
        return GWSTATUS_LEVEL_ERROR;
    
    Compiler *compilerForEntrant0, *compilerForEntrant1,
    *compilerForEntrant2, *compilerForEntrant3;
    
    std::vector<std::string> fileNames = getFilenamesOfAntPrograms();
    
//    if(fileNames.size()>=1){
//        compilerForEntrant0 = new Compiler;
//        if(!checkForCompilerError(compilerForEntrant0))
//            return GWSTATUS_LEVEL_ERROR;
//        else
//            m_anthill[0]->setCompiler(compilerForEntrant0);     //this anthill uses this compiler
//    }
//    if(fileNames.size()>=2){
//        compilerForEntrant1 = new Compiler;
//        if(!checkForCompilerError(compilerForEntrant1))
//            return GWSTATUS_LEVEL_ERROR;
//        else
//            m_anthill[1]->setCompiler(compilerForEntrant1);
//    }
//    if(fileNames.size()>=3){
//        compilerForEntrant2 = new Compiler;
//        if(!checkForCompilerError(compilerForEntrant2))
//            return GWSTATUS_LEVEL_ERROR;
//        else
//            m_anthill[2]->setCompiler(compilerForEntrant2);
//    }
//    if(fileNames.size()==4){
//        compilerForEntrant3 = new Compiler;
//        if(!checkForCompilerError(compilerForEntrant3))
//            return GWSTATUS_LEVEL_ERROR;
//        else
//            m_anthill[3]->setCompiler(compilerForEntrant3);
//    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    updateTicks(); //increments ticks by one and also sets each actor's didAct variable to false for the new tick
    displayGameText();
    
    for(int i=0; i<64; i++)
    {
        for(int j=0; j<64; j++)
        {
            list<Actor*>::iterator it;
            it = m_container[i][j].begin();
            while(it!=m_container[i][j].end())
            {
                Actor *q = *it;
                int oldX = q->getX();
                int oldY = q->getY();
                
                if(q->isAlive() && !q->hasDoneSomethingThisTick())
                {
                    (q)->doSomething();
                    int newX = q->getX();
                    int newY = q->getY();
                    if(newX != oldX || newY != oldY){    //object moved
                        Actor* temp = q;
                        m_container[newX][newY].push_back(temp);
                        temp->setActed(true);
                        list<Actor*>::iterator b = it;
                        it = m_container[i][j].erase(b);
                        it--;
                    }
                }
                else if(!q->isAlive())
                {
                    q->setVisible(false);
                    list<Actor*>::iterator b = it;
                    it = m_container[i][j].erase(b);
                    it--;
                }
                it++;
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for(int i=0; i<64; i++)
    {
        for(int j=0; j<64; j++)
        {
            list<Actor*>::iterator it;
            it = m_container[i][j].begin();
            while(it!=m_container[i][j].end())
            {
                it = m_container[i][j].erase(it);
                it--;
                it++;
            }
        }
    }

}
StudentWorld::~StudentWorld()
{
    cleanUp();
}



////////////////////////////////////////////////////////////////////////////////////////////////////////
//Container functions (list)
///////////////////////////////////////////////////////////////////////////////////////////////////////
list<Actor*>& StudentWorld::getObjectsAt(int x, int y)
{
    return m_container[x][y];
}

void StudentWorld::removeObjectFromSimulation(Actor *object, int x, int y)
{
    list<Actor*>::iterator it;
    it = m_container[x][y].begin();
    
    while(*it != object && it!=m_container[x][y].end())
    {
        it++;
    }
    if(*it == object)
        m_container[x][y].erase(it);
}

bool StudentWorld::hasPebbleAt(int x, int y, Actor::Direction curr)
{
    int x1 = x;
    int y1 = y;
    switch (curr)
    {
        case Actor::down:
            if(y>1){
                y1 = y-1;
            }
            else
                return true;
            break;
        case Actor::up:
            if(y<VIEW_HEIGHT-2){
                y1 = y+1;
            }
            else
                return true;
            break;
        case Actor::left:
            if(x>1){
                x1 = x-1;
            }
            else
                return true;
            break;
        case Actor::right:
            if(x<VIEW_WIDTH-2){
                x1 = x+1;
            }
            else
                return true;
            break;
    }
    for(list<Actor*>::iterator it = m_container[x1][y1].begin(); it!=m_container[x1][y1].end(); it++)
    {
        Actor* q = *it;
        if(typeid(*q)==typeid(Pebble))
            return true;
    }
    return false;
}

int StudentWorld::getCurrentTicks() const
{
    return m_ticks;
}

void StudentWorld::updateTicks()
{
    for(int i=0; i<64; i++)
    {
        for(int j=0; j<64; j++)
        {
            list<Actor*>::iterator it;
            it = m_container[i][j].begin();
            while(it!=m_container[i][j].end())
            {
                Actor* q = *it;
                q->setActed(false);
                it++;
            }
        }
    }
    m_ticks++;
}

int StudentWorld::getWinningAntNumber()
{
    int maximum = 0;
//    for(int i=0; i<m_numberOfAnthills; i++)
//    {
//        if(m_anthill[i]->getNumberOfAnts() > m_anthill[maximum]->getNumberOfAnts())
//        {
//            maximum = i;
//        }
//    }
    return maximum;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Field File Functions
///////////////////////////////////////////////////////////////////////////////////////////////////////

bool StudentWorld::checkForCompilerError(Compiler* c)
{
    std::vector<std::string> fileNames = getFilenamesOfAntPrograms();
    std::string error;
    
    if (!c->compile(fileNames[0], error) )
    {
        // entrant 0’s source code had a syntax error!
        
        // send this error to our framework to warn the user.
        // do it JUST like this!
        setError(fileNames[0] + " " + error);
        // return an error to tell our simulation framework
        // that something went wrong, and it’ll inform the user
        return false;
    }
    else
    {
        return true;
    }
}

bool StudentWorld::loadFieldFile()
{
    string fieldFileName = getFieldFilename();
    Field f;
    
    Anthill  *ah0, *ah1, *ah2, *ah3;
    
    Field::LoadResult loadedFieldSuccessfully = f.loadField(fieldFileName);
    
    if(loadedFieldSuccessfully==Field::load_fail_bad_format)
        return false;
    for(int i=0; i<64; i++)         //going through each column
    {
        for(int j=0; j<64; j++)     //each row in the ith column
        {
            switch(f.getContentsOf(i, j))
            {
                case Field::FieldItem::empty:
                    break;
//                case Field::FieldItem::anthill0:
//                    m_anthill[0] = new Anthill(nullptr, IID_ANT_TYPE0, i, j);
//                    ah0 = new Anthill(nullptr, 0, i, j);
//                    addObjectToSimulation(ah0, i, j);
//                case Field::FieldItem::anthill1:
//                    m_anthill[1] = new Anthill(nullptr, IID_ANT_TYPE1, i, j);
//                    ah1 = new Anthill(nullptr, 0, i, j);
//                    addObjectToSimulation(ah1, i, j);
//                case Field::FieldItem::anthill2:
//                    m_anthill[2] = new Anthill(nullptr, IID_ANT_TYPE2, i, j);
//                    ah2 = new Anthill(nullptr, 0, i, j);
//                    addObjectToSimulation(ah2, i, j);
//                case Field::FieldItem::anthill3:
//                    m_anthill[3] = new Anthill(nullptr, IID_ANT_TYPE3, i, j);
//                    ah3 = new Anthill(nullptr, 0, i, j);
//                    addObjectToSimulation(ah3, i, j);
                case Field::FieldItem::food:
                    m_container[i][j].push_back(new Food(this, i, j));
                    break;
                case Field::FieldItem::water:
                    m_container[i][j].push_back(new WaterPool(this, i, j));
                    break;
//                case Field::FieldItem::poison:
//                    //add to vector
                case Field::FieldItem::grasshopper:
                    m_container[i][j].push_back(new BabyGrassHopper(this,i,j));
                    break;
                case Field::FieldItem::rock:
                    m_container[i][j].push_back(new Pebble(this,i,j));
                    break;
            }
        }
    }
    return true;
}


void StudentWorld::displayGameText()
{
    int ticks = getCurrentTicks();
    
    int antsAnthill0 = -1, antsAnthill1 = -1, antsAnthill2 = -1, antsAnthill3 = -1;
    
    if(m_anthill[0]!=nullptr)
        antsAnthill0 = m_anthill[0]->getNumberOfAnts();
    if(m_anthill[1]!=nullptr)
        antsAnthill1 = m_anthill[1]->getNumberOfAnts();
    if(m_anthill[2]!=nullptr)
        antsAnthill2 = m_anthill[2]->getNumberOfAnts();
    if(m_anthill[3]!=nullptr)
        antsAnthill3 = m_anthill[3]->getNumberOfAnts();
    int winningAntNumber = getWinningAntNumber();
    
    string s = formatGameText(ticks, antsAnthill0, antsAnthill1, antsAnthill2, antsAnthill3, winningAntNumber);
    setGameStatText(s);
}


std::string StudentWorld::formatGameText(int ticks, int antsAnt0, int antsAnt1, int antsAnt2, int antsAnt3, int winningAntNumber)
{
    ostringstream oss;
    oss << "Ticks:" << setw(5) << ticks << " -  ";
    oss.fill('0');
    
    for(int i=0; i<4; i++)
    {
        switch(i){
            case 0:
                if(antsAnt0 != -1 && winningAntNumber ==0)
                    oss << m_anthill[0]->getColonyName() << "*: " << setw(2) << antsAnt0 << "  ";
                else if(antsAnt0!=-1)
                    oss << m_anthill[0]->getColonyName() << ": " << setw(2) << antsAnt0 << "  ";
                break;
            case 1:
                if(antsAnt1 != -1 && winningAntNumber ==1)
                    oss << m_anthill[1]->getColonyName() << "*: " << setw(2) << antsAnt1 << "  ";
                else if(antsAnt1!=-1)
                    oss << m_anthill[1]->getColonyName() << ": " << setw(2) << antsAnt1 << "  ";
                break;
            case 2:
                if(antsAnt2 != -1 && winningAntNumber ==2)
                    oss << m_anthill[2]->getColonyName() << "*: " << setw(2) << antsAnt2 << "  ";
                else if(antsAnt2!=-1)
                    oss << m_anthill[2]->getColonyName() << ": " << setw(2) << antsAnt2 << "  ";
                break;
            case 3:
                if(antsAnt3 != -1 && winningAntNumber ==3)
                    oss << m_anthill[3]->getColonyName() << "*: " << setw(2) << antsAnt3;
                else if(antsAnt3!=-1)
                    oss << m_anthill[3]->getColonyName() << ": " << setw(2) << antsAnt3;
                
                break;
        }
    }
    return oss.str();
}


GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Food Container Functions
//
//  -> These functions are used by food objects to monitor how much food is at a certain square and return that information to any energy holder (insects). These functions can also be used to change amount of food at any location in the StudentWorld (insects dying, insects consuming food)
///////////////////////////////////////////////////////////////////////////////////////////////////////

int StudentWorld::consumableFood(int x, int y, int units)
{
    list<Actor*>::iterator it;
    it = m_container[x][y].begin();
    while(it != m_container[x][y].end())
    {
        if((*it)->howMuchFoodHere() > 0)     //calls the derived class function on each object
        {
            if((*it)->howMuchFoodHere() >= units)
            {
                Food* q = dynamic_cast<Food*>(*it);
                q->decreaseEnergyBy(units);
                return units;
            }
            else
            {
                (*it)->setDead();   //all the food in this square was eaten
                return (*it)->howMuchFoodHere();
            }
        }
        it++;
    }
    return 0;
}

int StudentWorld::totalFood(int x, int y)
{
    list<Actor*>::iterator it;
    it = m_container[x][y].begin();
    while(it != m_container[x][y].end())
    {
        if((*it)->howMuchFoodHere() > 0)     //calls the derived class function on each object
        {
            return (*it)->howMuchFoodHere();
        }
        it++;
    }
    return 0;
}

Actor* StudentWorld::getFoodObject(int x, int y) //returns the actor object that the existing food object(if it exists) is derived from
{
    list<Actor*>::iterator it;
    it = m_container[x][y].begin();
    while(it != m_container[x][y].end())
    {
        if((*it)->howMuchFoodHere() > 0)     //calls the derived class function on each object
        {
            return *it;
        }
        it++;
    }
    return nullptr;
}

void StudentWorld::addFoodToSquare(int x, int y)
{
    int foodAlreadyThere = totalFood(x, y);
    if(getFoodObject(x, y) == nullptr)          //if there is no food object at this square
        m_container[x][y].push_back(new Food(this,x, y, 100));
    else{
        removeObjectFromSimulation(getFoodObject(x, y), x, y);
        m_container[x][y].push_back(new Food(this,x, y, 100+foodAlreadyThere));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Insect Container Functions
//
//  -> These functions are used by insects to get information on other objects in the same location and use that information to perform actions such as biting, or maturing into an adult at that location.
///////////////////////////////////////////////////////////////////////////////////////////////////////

void StudentWorld::bite(int strength, int x, int y, Insect* biter)     //the strength of each insect's bite is different
{
    vector<Actor*> insectsInSameSquare;
    vector<string> typeIDsOfEach;
    
    list <Actor*>::iterator it=m_container[x][y].begin();
    
    while(it!=m_container[x][y].end())
    {
        Actor* q = *it;
        if(q != nullptr)
            if(typeid(*q) == typeid(GrassHopper) ||typeid(*q) == typeid(BabyGrassHopper)){   //need to add ant
                if(typeid(*q) == typeid(BabyGrassHopper)){
                    typeIDsOfEach.push_back("BabyGrassHopper");
                    insectsInSameSquare.push_back(*it);
                }
                else if(typeid(*q) == typeid(GrassHopper) && dynamic_cast<GrassHopper*>(q)!= dynamic_cast<GrassHopper*>(biter)){
                    typeIDsOfEach.push_back("GrassHopper");
                    insectsInSameSquare.push_back(*it);
                }
            }

        it++;
    }
    if(insectsInSameSquare.size()>0)
    {
        int index = randInt(0, (insectsInSameSquare.size())-1);
        Actor* InsectToBite = insectsInSameSquare[index];
        
        if(typeIDsOfEach[index] == "BabyGrassHopper")
        {
            BabyGrassHopper* v = dynamic_cast<BabyGrassHopper*>(InsectToBite);
            v->setBitten(strength);
        }
        else if(typeIDsOfEach[index] == "GrassHopper")
        {
            GrassHopper* v = dynamic_cast<GrassHopper*>(InsectToBite);
            v->setBitten(strength);
        }
    }
}

void StudentWorld::becomeAdultGrassHopper(int x, int y)
{
    m_container[x][y].push_back(new GrassHopper(this, x, y));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Deterrent Container Functions
//
//  -> These functions are used by insects to get information on other objects in the same location and use that information to perform actions such as biting, or maturing into an adult at that location.
///////////////////////////////////////////////////////////////////////////////////////////////////////

void StudentWorld::stunInsect(int x, int y)
{
    int countInsects =0;
    string typeID = "";
    
    vector<Actor*> insectsInSameSquare;
    vector<string> typeIDsOfEach;
    
    list <Actor*>::iterator it=m_container[x][y].begin();
    
    while(it!=m_container[x][y].end())
    {
        Actor* q = *it;
        if(q != nullptr)
        {
            if(typeid(*q) == typeid(GrassHopper) ||typeid(*q) == typeid(BabyGrassHopper)){   //need to add ant
                if(typeid(*q) == typeid(BabyGrassHopper))
                    typeIDsOfEach.push_back("BabyGrassHopper");
                else if(typeid(*q) == typeid(GrassHopper))
                    typeIDsOfEach.push_back("GrassHopper");
                insectsInSameSquare.push_back(*it);
            }
        }
        it++;
    }
    if(insectsInSameSquare.size() > 0)
    {
        vector<Actor*>::iterator it1 = insectsInSameSquare.begin();
        vector<string>::iterator it2 = typeIDsOfEach.begin();
        while(it1!=insectsInSameSquare.end())
        {
            if(*it2 == "BabyGrassHopper"){
                BabyGrassHopper* v = dynamic_cast<BabyGrassHopper*>(*it1);
                if(!v->checkIfStunned())
                {
                    v->setStunned();
                }
            }
            else if(*it2 == "GrassHopper"){
                GrassHopper* v = dynamic_cast<GrassHopper*>(*it1);
                if(!v->checkIfStunned())
                {
                    v->setStunned();
                }
            }
            it1++;
            it2++;
        }
    }
}


