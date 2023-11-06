#ifndef gemsClass_hpp
#define gemsClass_hpp

#include <map>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "eventHandlerClass.hpp"
#include "colorIdentifier.h"

class Sn_EventHandler;

class Sn_StandardGem {
public:
	Sn_StandardGem();
	Sn_StandardGem(double xLoc, double yLoc, double size, const Sn_ColorIdentifier &col);
    
	virtual void draw(const std::map<Sn_ColorIdentifier, Sn_RGBColor> &colorMap);
    virtual std::pair<double, double> getLocation();
    virtual void processBonusInteraction(Sn_StandardGem** gemTab, std::vector<std::pair<Sn_StandardGem*, int>> &bonusesForProcessing, int rowNum, int colNum, Sn_EventHandler *handle);
    
	void swapAttributes(Sn_StandardGem* other);

	bool isScheduledForDeletion();
	void setScheduledForDeletion(bool deletion);

	void setNewCoords(double xNew, double yNew);

	Sn_ColorIdentifier getColor();
	void setColor(const Sn_ColorIdentifier& newCol);
protected:
    void setCoordsSquare();
    
	double m_xLoc;
	double m_yLoc;

	double m_size;
	Sn_ColorIdentifier m_color;
	bool m_scheduledForDeletion = 0;
};

class Sn_BonusGem : public Sn_StandardGem {
public:
	Sn_BonusGem(double xLoc, double yLoc, double size, const Sn_ColorIdentifier &col);
    void processBonusInteraction(Sn_StandardGem** gemTab, std::vector<std::pair<Sn_StandardGem*, int>> &bonusesForProcessing, int rowNum, int colNum, Sn_EventHandler *handle) override;
	void draw(const std::map<Sn_ColorIdentifier, Sn_RGBColor>& colorMap) override;
};

class Sn_ExplosiveBonus : public Sn_BonusGem
{
public:
    Sn_ExplosiveBonus(double xLoc, double yLoc, double size, const Sn_ColorIdentifier &col);
    void processBonusInteraction(Sn_StandardGem** gemTab, std::vector<std::pair<Sn_StandardGem*, int>> &bonusesForProcessing, int rowNum, int colNum, Sn_EventHandler *handle) override;
    void draw(const std::map<Sn_ColorIdentifier, Sn_RGBColor>& colorMap) override;
protected:
    void setCoordsBonusSquare();
};

class Sn_RecolorBonus : public Sn_BonusGem
{
public:
    Sn_RecolorBonus(double xLoc, double yLoc, double size, const Sn_ColorIdentifier &col);
    void processBonusInteraction(Sn_StandardGem** gemTab, std::vector<std::pair<Sn_StandardGem*, int>> &bonusesForProcessing, int rowNum, int colNum, Sn_EventHandler *handle) override;
    void draw(const std::map<Sn_ColorIdentifier, Sn_RGBColor>& colorMap) override;
protected:
    void setCoordsBonusTriangle();
};

#endif
