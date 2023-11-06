//
//  eventHandlerClass.hpp
//  CPPGemsGLUT
//
//  Created by Никита Шабанов on 05.11.2023.
//

#ifndef eventHandlerClass_hpp
#define eventHandlerClass_hpp

#include <array>
#include <random>
#include "gemsClass.h"

class Sn_StandardGem;

class Sn_EventHandler {
public:
    Sn_EventHandler(int gemRows, int gemColumns);
    ~Sn_EventHandler();
    
    void processBonusExplosive(Sn_StandardGem** gemTab, Sn_StandardGem *bonus, std::vector<std::pair<Sn_StandardGem*, int>> &bonusesForProcessing, int rowNum, int colNum);
    void processBonusRecolor(Sn_StandardGem** gemTab, Sn_StandardGem *bonus, std::vector<std::pair<Sn_StandardGem*, int>> &bonusesForProcessing, int rowNum, int colNum);
    void processBonusNoBonus(Sn_StandardGem** gemTab, Sn_StandardGem *bonus, std::vector<std::pair<Sn_StandardGem*, int>> &bonusesForProcessing, int rowNum, int colNum);
    static bool isNeighbour(int indexOne, int indexTwo, int gemRows);
private:
    int recountGems(Sn_StandardGem** gemTab);
    int recountGemsInRadius(Sn_StandardGem** gemTab, int radius, int columnOrig, int rowOrig);
    
    int m_gemRows;
    int m_gemColumns;
    std::mt19937 m_randomGen;
};


#endif /* eventHandlerClass_hpp */
