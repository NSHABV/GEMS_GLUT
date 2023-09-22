#include <algorithm>
#include <cstdint>
#include <iomanip>
#include "windowClass.h"

Sn_GemContainer::Sn_GemContainer(int xSize, int ySize, int gemSize) : m_windowX(xSize), m_windowY(ySize), m_gemSize(gemSize)
{
	std::random_device device;
	m_randomGen.seed(device());
	std::uniform_int_distribution<int> colorRange(0, 5);
	std::uniform_int_distribution<int> bonusChance(0, 100);

	m_gemTab = new Sn_StandardGem*[m_gemColumns * m_gemRows];

	for (int i = 0; i < m_gemColumns * m_gemRows; i++)
	{
		m_gemTab[i] = NULL;
	}

	refillMissingGems();
	generateColorMap();
}

Sn_GemContainer::~Sn_GemContainer()
{
	for (int i = 0; i < m_gemColumns * m_gemRows; i++)
	{
		if (m_gemTab[i] != NULL)
			delete m_gemTab[i];
	}
	delete m_gemTab;
}

void Sn_GemContainer::generateColorMap()
{
	Sn_RGBColor teal = {0, 128, 128};
	Sn_RGBColor crimson = {157, 34, 53};
	Sn_RGBColor purple = {128, 0, 128};
	Sn_RGBColor red = {255, 0, 0};
	Sn_RGBColor blue = {0, 0, 255};
	Sn_RGBColor green = {0, 255, 0};

	m_colorTab.insert(std::make_pair(Sn_ColorIdentifier::tealRGB, teal));
	m_colorTab.insert(std::make_pair(Sn_ColorIdentifier::crimsonRGB, crimson));
	m_colorTab.insert(std::make_pair(Sn_ColorIdentifier::purpleRGB, purple));
	m_colorTab.insert(std::make_pair(Sn_ColorIdentifier::redRGB, red));
	m_colorTab.insert(std::make_pair(Sn_ColorIdentifier::blueRGB, blue));
	m_colorTab.insert(std::make_pair(Sn_ColorIdentifier::greenRGB, green));
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
		if (m_gemTab[i] == NULL)
			continue;

		int currentRowCount = 1;
		Sn_ColorIdentifier lastCol = m_gemTab[i]->getColor();
		for (int a = 1; a < m_gemRows; a++)
		{
			if (m_gemTab[a * m_gemColumns + i] == NULL)
				continue;

			if (m_gemTab[a * m_gemColumns + i]->getColor() == lastCol)
			{
				currentRowCount++;
				if (a == m_gemRows - 1 && currentRowCount >= 3)
				{
                    /*
					for (auto j = a; j > a - currentRowCount; j--)
					{
						deletionIndexes.push_back(j * m_gemColumns + i);
						m_gemTab[j * m_gemColumns + i]->setScheduledForDeletion(true);
					}
                     */
                    deletionCheckRows(i, a, deletionIndexes, currentRowCount);
				}
			}
			else 
			{
				if (currentRowCount >= 3)
				{
                    /*
					for (auto j = a - 1; j > a - 1 - currentRowCount; j--)
					{
						deletionIndexes.push_back(j * m_gemColumns + i);
						m_gemTab[j * m_gemColumns + i]->setScheduledForDeletion(true);
					}
                     */
                    deletionCheckRows(i, a - 1, deletionIndexes, currentRowCount);
				}
				currentRowCount = 1;
				lastCol = m_gemTab[a * m_gemColumns + i]->getColor();
			}
		}
	}

	for (int i = 0; i < m_gemRows; i++)
	{
		if (m_gemTab[i * m_gemColumns] == NULL)
			continue;

		int currentRowCount = 1;
		Sn_ColorIdentifier lastCol = m_gemTab[i * m_gemColumns]->getColor();
		for (int a = 1; a < m_gemColumns; a++)
		{
			if (m_gemTab[i * m_gemColumns + a] == NULL)
				continue;

			if (m_gemTab[i * m_gemColumns + a]->getColor() == lastCol)
			{
				currentRowCount++;
				if (a == m_gemColumns - 1 && currentRowCount >= 3)
				{
                    /*
					for (auto j = a; j > a - currentRowCount; j--)
					{
						deletionIndexes.push_back(i * m_gemColumns + j);
						m_gemTab[i * m_gemColumns + j]->setScheduledForDeletion(true);
					}
                     */
                    deletionCheckColumns(i, a, deletionIndexes, currentRowCount);
				}
			}
			else
			{
				if (currentRowCount >= 3)
				{
                    /*
					for (auto j = a - 1; j > a - 1 - currentRowCount; j--)
					{
						deletionIndexes.push_back(i * m_gemColumns + j);
						m_gemTab[i * m_gemColumns + j]->setScheduledForDeletion(true);
					}
                     */
                    deletionCheckColumns(i, a - 1, deletionIndexes, currentRowCount);
				}
				currentRowCount = 1;
				lastCol = m_gemTab[i * m_gemColumns + a]->getColor();
			}
		}
	}

	if (deletionIndexes.size() > 0)
	{
		for (int i = 0; i < deletionIndexes.size(); i++)
		{
			int scheduledIndex = deletionIndexes.at(i);
			int columnNum = scheduledIndex % m_gemColumns;
			int rowNum = (scheduledIndex - columnNum) / m_gemColumns;

			if (columnNum < m_gemColumns - 1)
			{
                /*
				if (m_gemTab[scheduledIndex]->getColor() == m_gemTab[rowNum * m_gemColumns + columnNum + 1]->getColor() && !m_gemTab[rowNum * m_gemColumns + columnNum + 1]->isScheduledForDeletion())
				{
					m_gemTab[rowNum * m_gemColumns + columnNum + 1]->setScheduledForDeletion(true);
					deletionIndexes.push_back(rowNum * m_gemColumns + columnNum + 1);
				}
                 */
                sideCheck(rowNum, columnNum, scheduledIndex, deletionIndexes, sideId::right);
			}

			if (columnNum > 0)
			{
                /*
				if (m_gemTab[scheduledIndex]->getColor() == m_gemTab[rowNum * m_gemColumns + columnNum - 1]->getColor() && !m_gemTab[rowNum * m_gemColumns + columnNum - 1]->isScheduledForDeletion())
				{
					m_gemTab[rowNum * m_gemColumns + columnNum - 1]->setScheduledForDeletion(true);
					deletionIndexes.push_back(rowNum * m_gemColumns + columnNum - 1);
				}
                 */
                sideCheck(rowNum, columnNum, scheduledIndex, deletionIndexes, sideId::left);
			}

			if (rowNum < m_gemRows - 1)
			{
                /*
				if (m_gemTab[scheduledIndex]->getColor() == m_gemTab[(rowNum + 1) * m_gemColumns + columnNum]->getColor() && !m_gemTab[(rowNum + 1) * m_gemColumns + columnNum]->isScheduledForDeletion())
				{
					m_gemTab[(rowNum + 1) * m_gemColumns + columnNum]->setScheduledForDeletion(true);
					deletionIndexes.push_back((rowNum + 1)* m_gemColumns + columnNum);
				}
                 */
                sideCheck(rowNum, columnNum, scheduledIndex, deletionIndexes, sideId::bot);
			}

			if (rowNum > 0)
			{
                /*
				if (m_gemTab[scheduledIndex]->getColor() == m_gemTab[(rowNum - 1) * m_gemColumns + columnNum]->getColor() && !m_gemTab[(rowNum - 1) * m_gemColumns + columnNum]->isScheduledForDeletion())
				{
					m_gemTab[(rowNum - 1) * m_gemColumns + columnNum]->setScheduledForDeletion(true);
					deletionIndexes.push_back((rowNum - 1) * m_gemColumns + columnNum);
				}
                 */
                sideCheck(rowNum, columnNum, scheduledIndex, deletionIndexes, sideId::top);
			}
		}
	}

	for (int i : deletionIndexes)
	{
		if (m_gemTab[i] != NULL)
		{
			auto bonus = m_gemTab[i]->getBonusAttribute();
			if (bonus != Sn_BonusAttribute::nobonus)
				m_bonusesForProcessing.push_back(constructBonus(i));
			delete m_gemTab[i];
			m_gemTab[i] = NULL;
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

delayedBonus Sn_GemContainer::constructBonus(int index)
{
	delayedBonus bns;
	auto val = m_gemTab[index];
	if (val == NULL)
	{
		bns.colNum = -1;
		bns.origColor = Sn_ColorIdentifier::blueRGB;
		bns.rowNum = -1;
		bns.triggeredBonus = Sn_BonusAttribute::nobonus;
		return bns;
	}

	int columnNum = index % m_gemColumns;
	int rowNum = (index - columnNum) / m_gemColumns;

	bns.colNum = columnNum;
	bns.rowNum = rowNum;
	bns.triggeredBonus = val->getBonusAttribute();
	bns.origColor = val->getColor();

	return bns;
}

void Sn_GemContainer::processBonus(const delayedBonus& bonusForProcess)
{
	std::uniform_int_distribution<int> radiusThrow(-3, 3);
	std::uniform_int_distribution<int> randIndexThrow(0, m_gemRows * m_gemColumns - 1);
	auto newColumn = bonusForProcess.colNum + radiusThrow(m_randomGen);
	auto newRow = bonusForProcess.rowNum + radiusThrow(m_randomGen);
	newColumn = std::max(0, std::min(newColumn, m_gemColumns - 1));
	newRow = std::max(0, std::min(newRow, m_gemRows - 1));
	
	auto newIndex = newRow * m_gemColumns + newRow;
	while (m_gemTab[newIndex] == NULL)
	{
		newColumn = bonusForProcess.colNum + radiusThrow(m_randomGen);
		newRow = bonusForProcess.rowNum + radiusThrow(m_randomGen);
		newColumn = std::max(0, std::min(newColumn, m_gemColumns - 1));
		newRow = std::max(0, std::min(newRow, m_gemRows - 1));

		newIndex = newRow * m_gemColumns + newRow;
	}

	if (bonusForProcess.triggeredBonus == Sn_BonusAttribute::explosive)
	{
		if (m_gemTab[newIndex] != NULL)
		{
			auto bonus = m_gemTab[newIndex]->getBonusAttribute();
			if (bonus != Sn_BonusAttribute::nobonus)
				m_bonusesForProcessing.push_back(constructBonus(newIndex));
			delete m_gemTab[newIndex];
			m_gemTab[newIndex] = NULL;

			for (int deletedCounter = 0; deletedCounter < 4; deletedCounter++)
			{
				int newNdex = randIndexThrow(m_randomGen);
				while (m_gemTab[newNdex] == NULL)
					newNdex = randIndexThrow(m_randomGen);

				bonus = m_gemTab[newNdex]->getBonusAttribute();
				if (bonus != Sn_BonusAttribute::nobonus)
					m_bonusesForProcessing.push_back(constructBonus(newNdex));
				delete m_gemTab[newNdex];
				m_gemTab[newNdex] = NULL;
			}
		}
		m_recentlyReshaped = true;
	}
	else if (bonusForProcess.triggeredBonus == Sn_BonusAttribute::recolor)
	{
		if (m_gemTab[newIndex] != NULL)
		{
			m_gemTab[newIndex]->setColor(bonusForProcess.origColor);

			for (int deletedCounter = 0; deletedCounter < 2; deletedCounter++)
			{
				int newNdex = randIndexThrow(m_randomGen);
				while (m_gemTab[newNdex] == NULL || !isNeighbour(newNdex, newIndex))
					newNdex = randIndexThrow(m_randomGen);

				m_gemTab[newNdex]->setColor(bonusForProcess.origColor);
			}
		}

		m_recentlyReshaped = true;
	}
}

void Sn_GemContainer::bonusesProcessing()
{
	for (int i = 0; i < m_bonusesForProcessing.size(); i++)
		processBonus(m_bonusesForProcessing.at(i));
	m_bonusesForProcessing.clear();
}

bool Sn_GemContainer::isNeighbour(int indexOne, int indexTwo)
{
	int columnNumOne = indexOne % m_gemColumns;
	int rowNumOne = (indexOne - columnNumOne) / m_gemColumns;

	int columnNumTwo = indexTwo % m_gemColumns;
	int rowNumTwo = (indexTwo - columnNumTwo) / m_gemColumns;

	if (abs(columnNumOne - columnNumTwo) <= 1 && abs(rowNumOne - rowNumTwo) <= 1)
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
		if (m_gemTab[i] == NULL)
		{
			int isReplacementFound = 0;
			m_recentlyReshaped = true;

			for (int a = i - m_gemColumns; a >= 0; a -= m_gemColumns)
			{
				if (m_gemTab[a] != NULL)
				{
					int columnNum = i % m_gemColumns;
					int rowNum = (i - columnNum) / m_gemColumns;
					double x = -1.0 + 2.0 * double(m_gemSize) * double(columnNum) / double(m_windowX);
					double y = -1.0 + 2.0 * double(m_gemSize) * double(m_gemRows - rowNum - 1) / double(m_windowY);

					m_gemTab[i] = m_gemTab[a];
					m_gemTab[a] = NULL;
					m_gemTab[i]->setNewCoords(x, y);
					isReplacementFound = 1;
					break;
				}
			}

			if (!isReplacementFound)
			{
				auto bonusRoll = bonusChance(m_randomGen);
				auto col = static_cast<Sn_ColorIdentifier>(colorRange(m_randomGen));

				int columnNum = i % m_gemColumns;
				int rowNum = (i - columnNum) / m_gemColumns;
				double x = -1.0 + 2.0 * double(m_gemSize) * double(columnNum) / double(m_windowX);
				double y = -1.0 + 2.0 * double(m_gemSize) * double(m_gemRows - rowNum - 1) / double(m_windowY);
				double size = 2.0 * double(m_gemSize) / double(m_windowX);

				if (bonusRoll <= 98)
					m_gemTab[i] = new Sn_StandardGem(x, y, size, col);
				else if (bonusRoll <= 99)
				{
					auto bonusGem = new Sn_BonusGem(x, y, size, col, Sn_BonusAttribute::explosive);
					m_gemTab[i] = bonusGem;
				}
				else
				{
					auto bonusGem = new Sn_BonusGem(x, y, size, col, Sn_BonusAttribute::recolor);
					m_gemTab[i] = bonusGem;
				}
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
	int rowNum = (m_selectedGemIndex - columnNum) / m_gemColumns;
	int oldIndex = rowNum * m_gemColumns + columnNum;
	//if (abs(newColumnNum - columnNum) == 0 && abs(newRowNum - rowNum) <= 1 || abs(newColumnNum - columnNum) <= 1 && abs(newRowNum - rowNum) == 0)
    if (isNeighbour(newIndex, oldIndex))
	{
		auto tmp = m_gemTab[oldIndex];
		m_gemTab[oldIndex] = m_gemTab[newIndex];
		m_gemTab[newIndex] = tmp;
		m_gemTab[newIndex]->swapAttributes(m_gemTab[oldIndex]);
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
			if (m_gemTab[i] == NULL)
			{
				refillMissingGems();
				return;
			}
		}

		m_recentlyReshaped = destructionCheck();
	}
}
