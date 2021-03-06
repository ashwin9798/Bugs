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
    
    for(int i=0; i<4; i++)
    {
        numberOfAntsInEach[i] = -1;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Pure Virtual Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////

int StudentWorld::init()
{
    m_ticks = 0;
    
    Compiler* compilerForEntrant0 = nullptr;
    Compiler* compilerForEntrant1 = nullptr;
    Compiler* compilerForEntrant2 = nullptr;
    Compiler* compilerForEntrant3 = nullptr;
    std::vector<std::string> fileNames = getFilenamesOfAntPrograms();

    if(fileNames.size()>=1){
        compilerForEntrant0 = new Compiler;
        if(!checkForCompilerError(compilerForEntrant0,0))
            return GWSTATUS_LEVEL_ERROR;
    }
    if(fileNames.size()>=2){
        compilerForEntrant1 = new Compiler;
        if(!checkForCompilerError(compilerForEntrant1,1))
            return GWSTATUS_LEVEL_ERROR;
    }
    if(fileNames.size()>=3){
        compilerForEntrant2 = new Compiler;
        if(!checkForCompilerError(compilerForEntrant2,2))
            return GWSTATUS_LEVEL_ERROR;
    }
    if(fileNames.size()==4){
        compilerForEntrant3 = new Compiler;
        if(!checkForCompilerError(compilerForEntrant3,3))
            return GWSTATUS_LEVEL_ERROR;
    }
    
    if(!loadFieldFile(compilerForEntrant0, compilerForEntrant1, compilerForEntrant2, compilerForEntrant3))
        return GWSTATUS_LEVEL_ERROR;
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
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
                    delete(*it);
                    it = m_container[i][j].erase(it);
                    it--;
                }
                it++;
            }
        }
    }
    updateTicks(); //increments ticks by one and also sets each actor's didAct variable to false for the new tick
    if(m_ticks == 2000){
        if(getWinningAntNumber() == -1)
            return GWSTATUS_NO_WINNER;
        else{
            setWinner(namesOfAnthills[getWinningAntNumber()]);
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
                delete(*it);
                it = m_container[i][j].erase(it);
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
        default:
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
    vector<int> livingAnthills;
    for(int i=0; i<4; i++){
        if(numberOfAntsInEach[i]!=-1)
            livingAnthills.push_back(i);
    }
    bool noWinner = true;
    
    int maximum = livingAnthills[0];
    
    for(int i=0; i<livingAnthills.size(); i++)
    {
        if(numberOfAntsInEach[livingAnthills[i]] > numberOfAntsInEach[maximum])
        {
            if(numberOfAntsInEach[livingAnthills[i]] > 5)
            {
                maximum = livingAnthills[i];
                noWinner = false;
            }
        }
    }
    if(noWinner)
        return -1;
    return maximum;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Field File Functions
///////////////////////////////////////////////////////////////////////////////////////////////////////

bool StudentWorld::checkForCompilerError(Compiler* c, int n)
{
    std::vector<std::string> fileNames = getFilenamesOfAntPrograms();
    std::string error;
    
    if (!c->compile(fileNames[n], error) )
    {
        // entrant 0’s source code had a syntax error!
        
        // send this error to our framework to warn the user.
        // do it JUST like this!
        setError(fileNames[n] + " " + error);
        // return an error to tell our simulation framework
        // that something went wrong, and it’ll inform the user
        return false;
    }
    else
    {
        return true;
    }
}

bool StudentWorld::loadFieldFile(Compiler* c0, Compiler* c1, Compiler* c2, Compiler* c3)
{
    string fieldFileName = getFieldFilename();
    Field f;
    
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
                    
                case Field::FieldItem::anthill0:
                    if(c0 != nullptr){
                        m_container[i][j].push_back(new Anthill(this, c0, IID_ANT_TYPE0, i, j));
                        numberOfAntsInEach[0] = 0;
                        namesOfAnthills[0] = c0->getColonyName();
                        m_numberOfAnthills++;
                    }
                    break;
                    
                case Field::FieldItem::anthill1:
                    if(c1 != nullptr){
                        m_container[i][j].push_back(new Anthill(this, c1, IID_ANT_TYPE1, i, j));
                        numberOfAntsInEach[1] = 0;
                        namesOfAnthills[1] = c1->getColonyName();
                        m_numberOfAnthills++;
                    }
                    break;
                    
                case Field::FieldItem::anthill2:
                    if(c2 != nullptr){
                        m_container[i][j].push_back(new Anthill(this, c2, IID_ANT_TYPE2, i, j));
                        numberOfAntsInEach[2] = 0;
                        namesOfAnthills[2] = c2->getColonyName();
                        m_numberOfAnthills++;
                    }
                    break;
                    
                case Field::FieldItem::anthill3:
                    if(c3 != nullptr){
                        m_container[i][j].push_back(new Anthill(this, c3, IID_ANT_TYPE3, i, j));
                        numberOfAntsInEach[3] = 0;
                        namesOfAnthills[3] = c3->getColonyName();
                        m_numberOfAnthills++;
                    }
                    break;
                    
                case Field::FieldItem::food:
                    m_container[i][j].push_back(new Food(this, i, j));
                    break;
                    
                case Field::FieldItem::water:
                    m_container[i][j].push_back(new WaterPool(this, i, j));
                    break;
                    
                case Field::FieldItem::poison:
                    m_container[i][j].push_back(new Poison(this, i, j));
                    break;
                    
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
    
    if(numberOfAntsInEach[0] != -1)
        antsAnthill0 = numberOfAntsInEach[0];
    if(numberOfAntsInEach[1]!= -1)
        antsAnthill1 = numberOfAntsInEach[1];
    if(numberOfAntsInEach[2] != -1)
        antsAnthill2 = numberOfAntsInEach[2];
    if(numberOfAntsInEach[3]!= -1)
        antsAnthill3 = numberOfAntsInEach[3];
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
                    oss << namesOfAnthills[0] << "*: " << setw(2) << antsAnt0 << "  ";
                else if(antsAnt0!=-1)
                    oss << namesOfAnthills[0] << ": " << setw(2) << antsAnt0 << "  ";
                break;
            case 1:
                if(antsAnt1 != -1 && winningAntNumber ==1)
                    oss << namesOfAnthills[1] << "*: " << setw(2) << antsAnt1 << "  ";
                else if(antsAnt1!=-1)
                    oss << namesOfAnthills[1] << ": " << setw(2) << antsAnt1 << "  ";
                break;
            case 2:
                if(antsAnt2 != -1 && winningAntNumber ==2)
                    oss << namesOfAnthills[2] << "*: " << setw(2) << antsAnt2 << "  ";
                else if(antsAnt2!=-1)
                    oss << namesOfAnthills[2] << ": " << setw(2) << antsAnt2 << "  ";
                break;
            case 3:
                if(antsAnt3 != -1 && winningAntNumber ==3)
                    oss << namesOfAnthills[3] << "*: " << setw(2) << antsAnt3;
                else if(antsAnt3!=-1)
                    oss << namesOfAnthills[3] << ": " << setw(2) << antsAnt3;
                
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
                Actor* q = *it;
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

void StudentWorld::addFoodToSquare(int x, int y, int amount)
{
    int foodAlreadyThere = totalFood(x, y);
    if(getFoodObject(x, y) == nullptr)          //if there is no food object at this square
        m_container[x][y].push_back(new Food(this,x, y, amount));
    else{
        removeObjectFromSimulation(getFoodObject(x, y), x, y);
        m_container[x][y].push_back(new Food(this,x, y, amount+foodAlreadyThere));
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
    
    //Identify whether biter is an ant or a grasshopper
    bool biterIsAnt = false;
    if(dynamic_cast<Ant*>(biter) != nullptr)
        biterIsAnt = true;
    
    while(it!=m_container[x][y].end())
    {
        Actor* q = *it;
        if(q != nullptr)
            if(q->isInsect())
            {
                if(typeid(*q) == typeid(BabyGrassHopper)){
                    insectsInSameSquare.push_back(*it);
                }
                else if(typeid(*q) == typeid(GrassHopper)){
                    if(!biterIsAnt)
                    {
                        if(dynamic_cast<GrassHopper*>(q)!= dynamic_cast<GrassHopper*>(biter))
                            insectsInSameSquare.push_back(*it);
                        
                    }
                    else
                        insectsInSameSquare.push_back(*it);

                }
                else if(typeid(*q) == typeid(Ant)){
                    if(biterIsAnt){
                        Ant* beingBitten = dynamic_cast<Ant*>(q);
                        Ant* biting = dynamic_cast<Ant*>(biter);
                        if((beingBitten != biting) && (beingBitten->getColonyNumber() != biting->getColonyNumber())){
                            insectsInSameSquare.push_back(*it);
                        }
                    }
                    else
                        insectsInSameSquare.push_back(*it);
                }
            }

        it++;
    }
    if(insectsInSameSquare.size()>0)
    {
        int index = randInt(0, (insectsInSameSquare.size())-1);
        Actor* InsectToBite = insectsInSameSquare[index];
        
        InsectToBite->getBitten(strength);
    }
}

void StudentWorld::becomeAdultGrassHopper(int x, int y)
{
    m_container[x][y].push_back(new GrassHopper(this, x, y));
}

void StudentWorld::giveBirthToAnt(int x, int y, Compiler* c, int imageID, Anthill* hillPtr)
{
    m_container[x][y].push_back(new Ant(this, c, imageID, x, y));
    numberOfAntsInEach[imageID]++;
}

void StudentWorld::antDied(int colonyNumber)
{
    numberOfAntsInEach[colonyNumber]--;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Deterrent Container Functions
//
//  -> These functions are used by insects to get information on other objects in the same location and use that information to perform actions such as biting, or maturing into an adult at that location.
///////////////////////////////////////////////////////////////////////////////////////////////////////

void StudentWorld::harmInsect(int x, int y, bool isPool)
{
    int countInsects =0;
    string typeID = "";
    
    vector<Actor*> insectsInSameSquare;
    
    list <Actor*>::iterator it=m_container[x][y].begin();
    
    while(it!=m_container[x][y].end())
    {
        Actor* q = *it;
        if(q != nullptr)
        {
            if(q->isHarmableInsect()){
                insectsInSameSquare.push_back(*it);
            }
        }
        it++;
    }
    if(insectsInSameSquare.size() > 0)
    {
        vector<Actor*>::iterator it1 = insectsInSameSquare.begin();
        while(it1!=insectsInSameSquare.end())
        {
            Actor* v = *it1;
            if(isPool){
                v->getStunned();
            }
            else{
                v->getPoisoned();
            }
            
            it1++;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ant Container Functions
//
//  -> Emit a pheromone
///////////////////////////////////////////////////////////////////////////////////////////////////////

bool StudentWorld::emitOrDetectPheromone(int x, int y, int imageID, int colonyNumber, bool isEmitting)
{
    bool myPheromoneHere = false;
    list<Actor*>::iterator it;
    it = m_container[x][y].begin();
    
    Pheromone* q;
    
    while(it != m_container[x][y].end())
    {
        if((*it)->howManyPheromonesHere() > 0)     //calls the derived class function on each object
        {
            q = dynamic_cast<Pheromone*>(*it);
            if(isEmitting){
                if(q->getColonyNumber() == colonyNumber){
                    myPheromoneHere = true;
                    if(q->getEnergyUnits() >= 512)
                        q->increaseEnergyBy(768 - q->getEnergyUnits());
                    else
                        q->increaseEnergyBy(256);
                }
            }
            else
            {
                if(q->getColonyNumber() == colonyNumber)
                    return true;
            }
        }
        it++;
    }
    
    if(!myPheromoneHere && isEmitting)
    {
        m_container[x][y].push_back(new Pheromone(this, imageID, x, y));
        return true;
    }
    return false;
}

bool StudentWorld::checkDangerousObjects(int x, int y, int colonyNumber, bool onlyCheckInsects)
{
    list<Actor*>::iterator it;
    it = m_container[x][y].begin();
    
    while(it != m_container[x][y].end())
    {
        if((*it)->isDangerousToAnt(colonyNumber))     //calls the derived class function on each object
        {
            if(onlyCheckInsects){
                Actor* q = *it;
                if(q->isInsect())
                    return true;
            }
            else
            {
                return true;
            }
        }
        it++;
    }
    return false;
}

void StudentWorld::deleteAnthill(int colonyNumber)
{
    m_numberOfAnthills--;
}
