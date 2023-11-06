//
//  eventHandlerClass.cpp
//  CPPGemsGLUT
//
//  Created by Никита Шабанов on 05.11.2023.
//

#include "eventHandlerClass.hpp"
#include <set>

Sn_EventHandler::Sn_EventHandler(int gemRows, int gemColumns)
{
    m_gemRows = gemRows;
    m_gemColumns = gemColumns;
    
    std::random_device device;
    m_randomGen.seed(device());
}

Sn_EventHandler::~Sn_EventHandler()
{
    
}

void Sn_EventHandler::processBonusNoBonus(Sn_StandardGem** gemTab, Sn_StandardGem *bonus, std::vector<std::pair<Sn_StandardGem*, int>> &bonusesForProcessing, int rowNum, int colNum)
{
    
}

void Sn_EventHandler::processBonusExplosive(Sn_StandardGem** gemTab, Sn_StandardGem *bonus, std::vector<std::pair<Sn_StandardGem*, int>> &bonusesForProcessing, int rowNum, int colNum)
{
    std::uniform_int_distribution<int> radiusThrow(-3, 3);
    std::uniform_int_distribution<int> randIndexThrow(0, m_gemRows * m_gemColumns - 1);
    auto newColumn = colNum + radiusThrow(m_randomGen);
    auto newRow = rowNum + radiusThrow(m_randomGen);
    newColumn = std::max(0, std::min(newColumn, m_gemColumns - 1));
    newRow = std::max(0, std::min(newRow, m_gemRows - 1));
    
    auto remainingCount = recountGems(gemTab);
    if (remainingCount < 1)
        return;
    auto newIndex = newRow * m_gemColumns + newRow;
    while (gemTab[newIndex] == nullptr)
    {
        newColumn = colNum + radiusThrow(m_randomGen);
        newRow = rowNum + radiusThrow(m_randomGen);
        newColumn = std::max(0, std::min(newColumn, m_gemColumns - 1));
        newRow = std::max(0, std::min(newRow, m_gemRows - 1));

        newIndex = newRow * m_gemColumns + newColumn;
    }
    
    if (gemTab[newIndex] != nullptr)
    {
        auto nextBonus = gemTab[newIndex];
        auto makePair = std::make_pair(nextBonus, newIndex);
        bonusesForProcessing.push_back(makePair);
        gemTab[newIndex] = nullptr;

        std::set<int> indexSet;
        indexSet.insert(newIndex);
        auto neededCount = std::min(remainingCount - 1, 4);
        for (int deletedCounter = 0; deletedCounter < neededCount; deletedCounter++)
        {
            int newNdex = randIndexThrow(m_randomGen);
            while (gemTab[newNdex] == nullptr || indexSet.contains(newNdex))
                newNdex = randIndexThrow(m_randomGen);

            nextBonus = gemTab[newNdex];
            auto mkPair = std::make_pair(nextBonus, newNdex);
            bonusesForProcessing.push_back(mkPair);
            gemTab[newNdex] = nullptr;
            indexSet.insert(newNdex);
        }
        
        int pf = 0;
    }
    //m_recentlyReshaped = true;
}

void Sn_EventHandler::processBonusRecolor(Sn_StandardGem** gemTab, Sn_StandardGem *bonus, std::vector<std::pair<Sn_StandardGem*, int>> &bonusesForProcessing, int rowNum, int colNum)
{
    std::uniform_int_distribution<int> radiusThrow(-3, 3);
    std::uniform_int_distribution<int> randIndexThrow(0, m_gemRows * m_gemColumns - 1);
    auto newColumn = colNum + radiusThrow(m_randomGen);
    auto newRow = rowNum + radiusThrow(m_randomGen);
    newColumn = std::max(0, std::min(newColumn, m_gemColumns - 1));
    newRow = std::max(0, std::min(newRow, m_gemRows - 1));
    
    auto newIndex = newRow * m_gemColumns + newRow;
    auto remainingCount = recountGems(gemTab);
    if (remainingCount < 1)
        return;
    while (gemTab[newIndex] == nullptr)
    {
        newColumn = colNum + radiusThrow(m_randomGen);
        newRow = rowNum + radiusThrow(m_randomGen);
        newColumn = std::max(0, std::min(newColumn, m_gemColumns - 1));
        newRow = std::max(0, std::min(newRow, m_gemRows - 1));

        newIndex = newRow * m_gemColumns + newColumn;
    }

    if (gemTab[newIndex] != nullptr)
    {
        gemTab[newIndex]->setColor(bonus->getColor());
        auto neededCount = std::min(recountGemsInRadius(gemTab, 3, newColumn, newRow), 2);
        std::set<int> indexSet;
        indexSet.insert(newIndex);
        for (int deletedCounter = 0; deletedCounter < neededCount; deletedCounter++)
        {
            auto radiusColumn = newColumn + radiusThrow(m_randomGen);
            auto radiusRow = newRow + radiusThrow(m_randomGen);
            radiusColumn = std::max(0, std::min(radiusColumn, m_gemColumns - 1));
            radiusRow = std::max(0, std::min(radiusRow, m_gemRows - 1));
            
            int newNdex = newRow * m_gemColumns + newColumn;
            while (gemTab[newNdex] == nullptr || isNeighbour(newNdex, newIndex, m_gemColumns) || indexSet.contains(newNdex))
            {
                radiusColumn = newColumn + radiusThrow(m_randomGen);
                radiusRow = newRow + radiusThrow(m_randomGen);
                radiusColumn = std::max(0, std::min(radiusColumn, m_gemColumns - 1));
                radiusRow = std::max(0, std::min(radiusRow, m_gemRows - 1));
                
                newNdex = radiusRow * m_gemColumns + radiusColumn;
            }

            gemTab[newNdex]->setColor(bonus->getColor());
            indexSet.insert(newNdex);
        }
        int a = 0;
    }

    //m_recentlyReshaped = true;
}

bool Sn_EventHandler::isNeighbour(int indexOne, int indexTwo, int gemColumns)
{
    int columnNumOne = indexOne % gemColumns;
    int rowNumOne = (indexOne - columnNumOne) / gemColumns;

    int columnNumTwo = indexTwo % gemColumns;
    int rowNumTwo = (indexTwo - columnNumTwo) / gemColumns;

    if (abs(columnNumOne - columnNumTwo) + abs(rowNumOne - rowNumTwo) <= 1)
        return true;
    else
        return false;
}

int Sn_EventHandler::recountGems(Sn_StandardGem** gemTab)
{
    int recount = 0;
    for (int i = 0; i < m_gemRows * m_gemColumns; i++)
    {
        if (gemTab[i] != nullptr)
            recount++;
    }
    return recount;
}

int Sn_EventHandler::recountGemsInRadius(Sn_StandardGem** gemTab, int radius, int columnOrig, int rowOrig)
{
    int recount = 0;
    
    for (auto colAdd = std::max(columnOrig - radius, 0); colAdd < std::min(columnOrig + radius, m_gemColumns); colAdd++)
    {
        for (auto rowAdd = std::max(rowOrig - radius, 0); rowAdd < std::min(rowOrig + radius, m_gemRows); rowAdd++)
        {
            int newNdex = rowAdd * m_gemColumns + colAdd;
            if (gemTab[newNdex] != nullptr)
                recount++;
        }
    }
    return recount;
}
