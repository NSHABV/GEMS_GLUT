#pragma once

#include <map>
#include <GL/glew.h>
#include <GL/freeglut.h>

enum Sn_BonusAttribute {
	nobonus,
	explosive,
	recolor
};

enum Sn_ColorIdentifier {
	redRGB,
	blueRGB,
	greenRGB,
	tealRGB,
	crimsonRGB,
	purpleRGB
};

struct Sn_RGBColor {
	int redRGB;
	int greenRGB;
	int blueRGB;
};

class Sn_StandardGem {
public:
	Sn_StandardGem();
	Sn_StandardGem(double xLoc, double yLoc, double size, const Sn_ColorIdentifier &col);

	virtual Sn_BonusAttribute getBonusAttribute();
	virtual void draw(const std::map<Sn_ColorIdentifier, Sn_RGBColor> &colorMap);
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
	Sn_BonusGem(double xLoc, double yLoc, double size, const Sn_ColorIdentifier &col, const Sn_BonusAttribute &bonus);

    virtual Sn_BonusAttribute getBonusAttribute() override;
	virtual void draw(const std::map<Sn_ColorIdentifier, Sn_RGBColor>& colorMap) override;
protected:
	Sn_BonusAttribute m_bonus;
    void setCoordsBonusSquare();
    void setCoordsBonusTriangle();
};
