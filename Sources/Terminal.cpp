//
// Created by alex on 9/20/26.
//
#include "../Headers/Terminal.h"
#include "../Headers/OutColour.h"

static bool isDefault(Colour clr) {
    if ((clr.fRGB.r == -1 && clr.fRGB.g == -1 && clr.fRGB.b == -1 && clr.bRGB.r == -1 && clr.bRGB.g == -1 && clr.bRGB.b == -1)
        && (clr.fC == OutputColour::None && clr.bC == OutputColour::None))
        return true;

    return false;
}

void error(const std::string &errorMsg, Colour clr) {
    if (isDefault(clr)) {
        clr.fC = OutputColour::BrightRed;
        setOutColour(errorMsg, clr.fC, clr.bC, {}, {}, true, 1);
        return;
    }
    setOutColour(errorMsg, OutputColour::None, OutputColour::None, clr.fRGB, clr.bRGB, true, 1);
}

void warn(const std::string &warnMsg, Colour clr) {
    if (isDefault(clr)) {
        clr.fC = OutputColour::BrightYellow;
        setOutColour(warnMsg, clr.fC, clr.bC, {}, {}, true, 1);
        return;
    }
    setOutColour(warnMsg, OutputColour::None, OutputColour::None, clr.fRGB, clr.bRGB, true, 1);
}
