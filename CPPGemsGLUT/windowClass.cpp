#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <chrono>
#include <thread>
#include "windowClass.h"

Sn_GemContainer::Sn_GemContainer(int xSize, int ySize, int gemSize) : m_windowX(xSize), m_windowY(ySize), m_gemSize(gemSize)
{
	std::random_device device;
	m_randomGen.seed(device());

	m_gemTab = new Sn_StandardGem*[m_gemColumns * m_gemRows];
    m_handle = new Sn_EventHandler(m_gemRows, m_gemColumns);

	for (int i = 0; i < m_gemColumns * m_gemRows; i++)
	{
		m_gemTab[i] = nullptr;
	}

	refillMissingGems();
	generateColorMap();
}

Sn_GemContainer::~Sn_GemContainer()
{
	for (int i = 0; i < m_gemColumns * m_gemRows; i++)
	{
		if (m_gemTab[i] != nullptr)
			delete m_gemTab[i];
	}
	delete m_gemTab;
    delete m_handle;
}

void Sn_GemContainer::generateColorMap()
{
	Sn_RGBColor teal = {0, 128, 128};
	Sn_RGBColor crimson = {157, 34, 53};
	Sn_RGBColor purple = {128, 0, 128};
	Sn_RGBColor red = {255, 0, 0};
	Sn_RGBColor blue = {0, 0, 255};
	Sn_RGBColor green = {0, 255, 0};
    Sn_RGBColor yellow = {255, 255, 0};

	m_colorTab.insert(std::make_pair(Sn_ColorIdentifier::tealRGB, teal));
	m_colorTab.insert(std::make_pair(Sn_ColorIdentifier::crimsonRGB, crimson));
	m_colorTab.insert(std::make_pair(Sn_ColorIdentifier::purpleRGB, purple));
	m_colorTab.insert(std::make_pair(Sn_ColorIdentifier::redRGB, red));
	m_colorTab.insert(std::make_pair(Sn_ColorIdentifier::blueRGB, blue));
	m_colorTab.insert(std::make_pair(Sn_ColorIdentifier::greenRGB, green));
    m_colorTab.insert(std::make_pair(Sn_ColorIdentifier::yellowRGB, yellow));
}


Sn_StandardGem** Sn_GemContainer::getTab()
{
	return m_gemTab;
}

void Sn_GemContainer::drawGemBoundary()
{
    int i = m_selectedGemIndex;
    int columnNum = i % m_gemColumns;
    int rowNum = (i - columnNum) / m_gemColumns;
    double x = -1.0 + 2.0 * double(m_gemSize) * double(columnNum) / double(m_windowX);
    double y = -1.0 + 2.0 * double(m_gemSize) * double(m_gemRows - rowNum - 1) / double(m_windowY);
    double size = double(m_gemSize) / double(m_windowX) * 2;

    glBegin(GL_LINE_STRIP);
    glColor3f(255, 255, 255);
    glPointSize(10);
    glVertex2d(x, y);
    glVertex2d(x + size, y);
    glVertex2d(x + size, y + size);
    glVertex2d(x, y + size);
    glVertex2d(x, y);
    glPointSize(1);
    glEnd();
}

void Sn_GemContainer::drawGems()
{
	for (int i = 0; i < m_gemColumns * m_gemRows; i++)
	{
		if (m_gemTab[i])
			m_gemTab[i]->draw(m_colorTab);
	}

	if (m_selectedGemIndex != -1)
        drawGemBoundary();
}

void Sn_GemContainer::deletionCheckRows(int i, int a, std::vector<int> &indexes, int currentRowCount)
{
    for (auto j = a; j > a - currentRowCount; j--)
    {
        indexes.push_back(j * m_gemColumns + i);
        m_gemTab[j * m_gemColumns + i]->setScheduledForDeletion(true);
    }
}

void Sn_GemContainer::deletionCheckColumns(int i, int a, std::vector<int> &indexes, int currentColumnCount)
{
    for (auto j = a; j > a - currentColumnCount; j--)
    {
        indexes.push_back(i * m_gemColumns + j);
        m_gemTab[i * m_gemColumns + j]->setScheduledForDeletion(true);
    }
}

void Sn_GemContainer::sideCheck(int rowNum, int columnNum, int scheduledIndex, std::vector<int> &deletionIndexes, sideId side)
{
    int otherIndex;
    switch(side)
    {
        case sideId::right:
            otherIndex = rowNum * m_gemColumns + columnNum + 1;
            break;
        case sideId::left:
            otherIndex = rowNum * m_gemColumns + columnNum - 1;
            break;
        case sideId::top:
            otherIndex = (rowNum - 1) * m_gemColumns + columnNum;
            break;
        case sideId::bot:
            otherIndex = (rowNum + 1) * m_gemColumns + columnNum;
            break;
        default:
            return;
    }
    
    if (m_gemTab[scheduledIndex]->getColor() == m_gemTab[otherIndex]->getColor() && !m_gemTab[otherIndex]->isScheduledForDeletion())
    {
        m_gemTab[otherIndex]->setScheduledForDeletion(true);
        deletionIndexes.push_back(otherIndex);
    }
}


bool Sn_GemContainer::destructionCheck()
{
	std::vector<int> deletionIndexes;
	for (int i = 0; i < m_gemColumns; i++)
	{
		if (m_gemTab[i] == nullptr)
			continue;

		int currentRowCount = 1;
		Sn_ColorIdentifier lastCol = m_gemTab[i]->getColor();
		for (int a = 1; a < m_gemRows; a++)
		{
			if (m_gemTab[a * m_gemColumns + i] == nullptr)
				continue;

			if (m_gemTab[a * m_gemColumns + i]->getColor() == lastCol)
			{
				currentRowCount++;
				if (a == m_gemRows - 1 && currentRowCount >= 3)
				{
                    deletionCheckRows(i, a, deletionIndexes, currentRowCount);
				}
			}
			else 
			{
				if (currentRowCount >= 3)
				{
                    deletionCheckRows(i, a - 1, deletionIndexes, currentRowCount);
				}
				currentRowCount = 1;
				lastCol = m_gemTab[a * m_gemColumns + i]->getColor();
			}
		}
	}

	for (int i = 0; i < m_gemRows; i++)
	{
		if (m_gemTab[i * m_gemColumns] == nullptr)
			continue;

		int currentRowCount = 1;
		Sn_ColorIdentifier lastCol = m_gemTab[i * m_gemColumns]->getColor();
		for (int a = 1; a < m_gemColumns; a++)
		{
			if (m_gemTab[i * m_gemColumns + a] == nullptr)
				continue;

			if (m_gemTab[i * m_gemColumns + a]->getColor() == lastCol)
			{
				currentRowCount++;
				if (a == m_gemColumns - 1 && currentRowCount >= 3)
				{
                    deletionCheckColumns(i, a, deletionIndexes, currentRowCount);
				}
			}
			else
			{
				if (currentRowCount >= 3)
				{
                    deletionCheckColumns(i, a - 1, deletionIndexes, currentRowCount);
				}
				currentRowCount = 1;
				lastCol = m_gemTab[i * m_gemColumns + a]->getColor();
			}
		}
	}

	for (int i = 0; i < deletionIndexes.size(); i++)
    {
        int scheduledIndex = deletionIndexes.at(i);
        int columnNum = scheduledIndex % m_gemColumns;
        int rowNum = (scheduledIndex - columnNum) / m_gemColumns;

        if (columnNum < m_gemColumns - 1)
            sideCheck(rowNum, columnNum, scheduledIndex, deletionIndexes, sideId::right);

        if (columnNum > 0)
            sideCheck(rowNum, columnNum, scheduledIndex, deletionIndexes, sideId::left);

        if (rowNum < m_gemRows - 1)
            sideCheck(rowNum, columnNum, scheduledIndex, deletionIndexes, sideId::bot);

        if (rowNum > 0)
            sideCheck(rowNum, columnNum, scheduledIndex, deletionIndexes, sideId::top);
    }

	for (int i : deletionIndexes)
	{
		if (m_gemTab[i] != nullptr)
		{
            auto makePair = std::make_pair(m_gemTab[i], i);
            m_gemsForBonusProcessing.push_back(makePair);
			m_gemTab[i] = nullptr;
		}
	}

	if (deletionIndexes.size() == 0)
		return false;
	else
	{
		bonusesProcessing();
		return true;
	}
}

void Sn_GemContainer::bonusesProcessing()
{
	for (int i = 0; i < m_gemsForBonusProcessing.size(); i++)
    {
        int columnNum = m_gemsForBonusProcessing.at(i).second % m_gemColumns;
        int rowNum = (m_gemsForBonusProcessing.at(i).second - columnNum) / m_gemRows;
        m_gemsForBonusProcessing.at(i).first->processBonusInteraction(m_gemTab, m_gemsForBonusProcessing, rowNum, columnNum, m_handle);
    }
    for (auto gem : m_gemsForBonusProcessing)
        delete gem.first;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    m_gemsForBonusProcessing.clear();
    m_recentlyReshaped = true;
}

bool Sn_GemContainer::isNeighbour(int indexOne, int indexTwo)
{
	int columnNumOne = indexOne % m_gemColumns;
	int rowNumOne = (indexOne - columnNumOne) / m_gemRows;

	int columnNumTwo = indexTwo % m_gemColumns;
	int rowNumTwo = (indexTwo - columnNumTwo) / m_gemRows;

	//if (abs(columnNumOne - columnNumTwo) <= 1 && abs(rowNumOne - rowNumTwo) <= 1)
    if (abs(columnNumOne - columnNumTwo) + abs(rowNumOne - rowNumTwo) <= 1)
		return true;
	else
		return false;
}

void Sn_GemContainer::refillMissingGems()
{
	std::random_device device;
	m_randomGen.seed(device());
	std::uniform_int_distribution<int> colorRange(0, 5);
	std::uniform_int_distribution<int> bonusChance(0, 100);

	for (int i = m_gemColumns * m_gemRows - 1; i >= 0; i--)
	{
        if (m_gemTab[i] != nullptr)
            continue;

        int isReplacementFound = 0;
        m_recentlyReshaped = true;
        
        int columnNum = i % m_gemColumns;
        int rowNum = (i - columnNum) / m_gemRows;
        double x = -1.0 + 2.0 * double(m_gemSize) * double(columnNum) / double(m_windowX);
        double y = -1.0 + 2.0 * double(m_gemSize) * double(m_gemRows - rowNum - 1) / double(m_windowY);
        for (int a = i - m_gemColumns; a >= 0; a -= m_gemColumns)
        {
            if (m_gemTab[a] == nullptr)
                continue;

            m_gemTab[i] = m_gemTab[a];
            m_gemTab[a] = nullptr;
            m_gemTab[i]->setNewCoords(x, y);
            isReplacementFound = 1;
            break;
            
        }

        if (!isReplacementFound)
        {
            auto bonusRoll = bonusChance(m_randomGen);
            auto col = static_cast<Sn_ColorIdentifier>(colorRange(m_randomGen));

            double size = 2.0 * double(m_gemSize) / double(m_windowX);

            if (bonusRoll <= 98)
                m_gemTab[i] = new Sn_StandardGem(x, y, size, col);
            else if (bonusRoll <= 99)
            {
                auto bonusGem = new Sn_ExplosiveBonus(x, y, size, col);
                m_gemTab[i] = bonusGem;
            }
            else
            {
                auto bonusGem = new Sn_RecolorBonus(x, y, size, col);
                m_gemTab[i] = bonusGem;
            }
        }
	}
}

void Sn_GemContainer::handleMouseClick(int x, int y)
{
	if (m_recentlyReshaped)
		return;

	int newColumnNum = (x - x % m_gemSize) / m_gemSize;
	int newRowNum = (y - y % m_gemSize) / m_gemSize;
	int newIndex = newRowNum * m_gemColumns + newColumnNum;
	
	int columnNum = m_selectedGemIndex % m_gemColumns;
	int rowNum = (m_selectedGemIndex - columnNum) / m_gemRows;
	int oldIndex = rowNum * m_gemColumns + columnNum;
    
    if (isNeighbour(newIndex, oldIndex))
	{
		auto tmp = m_gemTab[oldIndex];
		m_gemTab[oldIndex] = m_gemTab[newIndex];
		m_gemTab[newIndex] = tmp;
		m_gemTab[newIndex]->swapAttributes(m_gemTab[oldIndex]);
        
        m_selectedOldGemIndex = m_selectedGemIndex;
	}
	m_selectedGemIndex = newIndex;

	m_recentlyReshaped = true;
}

void Sn_GemContainer::handleRightMouseClick(int x, int y)
{
	if (m_recentlyReshaped)
		return;

	int newColumnNum = (x - x % m_gemSize) / m_gemSize;
	int newRowNum = (y - y % m_gemSize) / m_gemSize;
	int newIndex = newRowNum * m_gemColumns + newColumnNum;

	m_selectedGemIndex = newIndex;
}

int Sn_GemContainer::getXSize()
{
	return m_windowX;
}

int Sn_GemContainer::getGemSize()
{
	return m_gemSize;
}

int Sn_GemContainer::getYSize()
{
	return m_windowY;
}

void Sn_GemContainer::idleFunc()
{
	if (m_recentlyReshaped)
	{
		for (int i = 0; i < m_gemColumns * m_gemRows; i++)
		{
			if (m_gemTab[i] == nullptr)
			{
				refillMissingGems();
				return;
			}
		}

		m_recentlyReshaped = destructionCheck();
        if (!m_recentlyReshaped && m_selectedOldGemIndex != -1)
        {
            auto tmp = m_gemTab[m_selectedGemIndex];
            m_gemTab[m_selectedGemIndex] = m_gemTab[m_selectedOldGemIndex];
            m_gemTab[m_selectedOldGemIndex] = tmp;
            m_gemTab[m_selectedOldGemIndex]->swapAttributes(m_gemTab[m_selectedGemIndex]);
            
            m_selectedGemIndex = m_selectedOldGemIndex;
            m_selectedOldGemIndex = -1;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        else
            m_selectedOldGemIndex = -1;
	}
}
