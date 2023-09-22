#pragma once

#include <array>
#include <random>
#include "gemsClass.h"

struct delayedBonus
{
	Sn_BonusAttribute triggeredBonus;
	int colNum;
	int rowNum;
	Sn_ColorIdentifier origColor;
};

enum sideId
{
    left,
    right,
    top,
    bot
};

class Sn_GemContainer {
public:
	Sn_GemContainer(int xSize, int ySize, int gemSize);
	~Sn_GemContainer();

	void generateColorMap();
	
	bool destructionCheck();
	void refillMissingGems();

	void drawGems();

	Sn_StandardGem** getTab();
	int getXSize();
	int getGemSize();
	int getYSize();

	void handleMouseClick(int x, int y);
	void handleRightMouseClick(int x, int y);

	void idleFunc();
	void processBonus(const delayedBonus& bonusForProcess);

	delayedBonus constructBonus(int index);
	bool isNeighbour(int indexOne, int indexTwo);
	void bonusesProcessing();
private:
	int m_gemSize;

	int m_windowX;
	int m_windowY;
	int m_gemRows = 9;
	int m_gemColumns = 9;

	Sn_StandardGem** m_gemTab;
	std::map<Sn_ColorIdentifier, Sn_RGBColor> m_colorTab;

	std::mt19937 m_randomGen;

	int m_selectedGemIndex = 0;
	int m_recentlyReshaped = false;

	std::vector<delayedBonus> m_bonusesForProcessing;
    
    void drawGemBoundary();
    void sideCheck(int rowNum, int columnNum, int scheduledIndex, std::vector<int> &deletionIndexes, sideId side);
    
    void deletionCheckRows(int i, int a, std::vector<int> &indexes, int currentRowCount);
    void deletionCheckColumns(int i, int a, std::vector<int> &indexes, int currentColumnCount);
};
