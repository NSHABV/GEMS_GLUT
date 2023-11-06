//
//  colorIdentifier.h
//  CPPGemsGLUT
//
//  Created by Никита Шабанов on 06.11.2023.
//

#ifndef colorIdentifier_h
#define colorIdentifier_h

enum Sn_ColorIdentifier {
    redRGB,
    blueRGB,
    greenRGB,
    tealRGB,
    crimsonRGB,
    purpleRGB,
    yellowRGB
};

struct Sn_RGBColor {
    int redRGB;
    int greenRGB;
    int blueRGB;
};

#endif /* colorIdentifier_h */
