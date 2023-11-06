#include "gemsClass.h"
Sn_StandardGem::Sn_StandardGem()
{
}

Sn_StandardGem::Sn_StandardGem(double xLoc, double yLoc, double size, const Sn_ColorIdentifier& col)
{
	m_xLoc = xLoc;
	m_yLoc = yLoc;

	m_size = size;
	m_color = col;
}

void Sn_StandardGem::draw(const std::map<Sn_ColorIdentifier, Sn_RGBColor>& colorMap)
{
	glBegin(GL_POLYGON);
	Sn_RGBColor col = colorMap.at(m_color);
	glColor3f(double(col.redRGB) / 255.0, double(col.greenRGB) / 255.0, double(col.blueRGB) / 255.0);
    setCoordsSquare();
	glEnd();
}

Sn_ColorIdentifier Sn_StandardGem::getColor()
{
	return m_color;
}

bool Sn_StandardGem::isScheduledForDeletion()
{
	return m_scheduledForDeletion;
}

void Sn_StandardGem::setScheduledForDeletion(bool deletion)
{
	m_scheduledForDeletion = deletion;
}

void Sn_StandardGem::setNewCoords(double xNew, double yNew)
{
	m_xLoc = xNew;
	m_yLoc = yNew;
}

void Sn_StandardGem::swapAttributes(Sn_StandardGem* other)
{
	if (other == NULL)
		return;

	auto tempXLoc = m_xLoc;
	auto tempYLoc = m_yLoc;

	m_xLoc = other->m_xLoc;
	m_yLoc = other->m_yLoc;

	other->m_xLoc = tempXLoc;
	other->m_yLoc = tempYLoc;
}

void Sn_StandardGem::setColor(const Sn_ColorIdentifier& newCol)
{
	m_color = newCol;
}

void Sn_StandardGem::setCoordsSquare()
{
    glVertex2f(m_xLoc, m_yLoc);
    glVertex2f(m_xLoc, m_yLoc + m_size);
    glVertex2f(m_xLoc + m_size, m_yLoc + m_size);
    glVertex2f(m_xLoc + m_size, m_yLoc);
}

std::pair<double, double> Sn_StandardGem::getLocation()
{
    return std::make_pair(m_xLoc, m_yLoc);
}

Sn_BonusGem::Sn_BonusGem(double xLoc, double yLoc, double size, const Sn_ColorIdentifier& col)
{
	m_xLoc = xLoc;
	m_yLoc = yLoc;

	m_size = size;
	m_color = col;
}

void Sn_BonusGem::draw(const std::map<Sn_ColorIdentifier, Sn_RGBColor>& colorMap)
{
	glBegin(GL_POLYGON);
	Sn_RGBColor col = colorMap.at(m_color);
	glColor3f(double(col.redRGB) / 255.0, double(col.greenRGB) / 255.0, double(col.blueRGB) / 255.0);
    setCoordsSquare();
	glEnd();
}

Sn_ExplosiveBonus::Sn_ExplosiveBonus(double xLoc, double yLoc, double size, const Sn_ColorIdentifier& col) :
    Sn_BonusGem(xLoc, yLoc, size, col)
{
    m_xLoc = xLoc;
    m_yLoc = yLoc;

    m_size = size;
    m_color = col;
}

Sn_RecolorBonus::Sn_RecolorBonus(double xLoc, double yLoc, double size, const Sn_ColorIdentifier& col) :
    Sn_BonusGem(xLoc, yLoc, size, col)
{
    m_xLoc = xLoc;
    m_yLoc = yLoc;

    m_size = size;
    m_color = col;
}

void Sn_ExplosiveBonus::setCoordsBonusSquare()
{
    glVertex2f(m_xLoc + m_size / 3.0, m_yLoc + m_size / 3.0);
    glVertex2f(m_xLoc + m_size / 3.0, m_yLoc + 2.0 * m_size / 3.0);
    glVertex2f(m_xLoc + 2.0 * m_size / 3.0, m_yLoc + 2.0 * m_size / 3.0);
    glVertex2f(m_xLoc + 2.0 * m_size / 3.0, m_yLoc + m_size / 3.0);
}

void Sn_RecolorBonus::setCoordsBonusTriangle()
{
    glVertex2f(m_xLoc + m_size / 2.0, m_yLoc + 2 * m_size / 3.0);
    glVertex2f(m_xLoc + m_size / 3.0, m_yLoc + m_size / 3.0);
    glVertex2f(m_xLoc + 2.0 * m_size / 3.0, m_yLoc + m_size / 3.0);
}

void Sn_ExplosiveBonus::draw(const std::map<Sn_ColorIdentifier, Sn_RGBColor>& colorMap)
{
    Sn_BonusGem::draw(colorMap);
    
    glBegin(GL_POLYGON);
    glColor3f(255, 255, 255);
    setCoordsBonusSquare();
    glEnd();
}

void Sn_RecolorBonus::draw(const std::map<Sn_ColorIdentifier, Sn_RGBColor>& colorMap)
{
    Sn_BonusGem::draw(colorMap);
    
    glBegin(GL_TRIANGLES);
    glColor3f(255, 255, 255);
    setCoordsBonusTriangle();
    glEnd();
}

void Sn_RecolorBonus::processBonusInteraction(Sn_StandardGem** gemTab, std::vector<std::pair<Sn_StandardGem*, int>> &bonusesForProcessing, int rowNum, int colNum, Sn_EventHandler *handle)
{
    handle->processBonusRecolor(gemTab, this, bonusesForProcessing, rowNum, colNum);
}

void Sn_StandardGem::processBonusInteraction(Sn_StandardGem** gemTab, std::vector<std::pair<Sn_StandardGem*, int>> &bonusesForProcessing, int rowNum, int colNum, Sn_EventHandler *handle)
{
    handle->processBonusNoBonus(gemTab, this, bonusesForProcessing, rowNum, colNum);
}

void Sn_BonusGem::processBonusInteraction(Sn_StandardGem** gemTab, std::vector<std::pair<Sn_StandardGem*, int>> &bonusesForProcessing, int rowNum, int colNum, Sn_EventHandler *handle)
{
    handle->processBonusNoBonus(gemTab, this, bonusesForProcessing, rowNum, colNum);
}

void Sn_ExplosiveBonus::processBonusInteraction(Sn_StandardGem** gemTab, std::vector<std::pair<Sn_StandardGem*, int>> &bonusesForProcessing, int rowNum, int colNum, Sn_EventHandler *handle)
{
    handle->processBonusExplosive(gemTab, this, bonusesForProcessing, rowNum, colNum);
}
