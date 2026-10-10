//
// Created by alex on 9/20/26.
//

#ifndef CTRACK_OUTPUT_H
#define CTRACK_OUTPUT_H
#include <string>

enum class OutputColour : u_int8_t {
    Black,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,

    BrightBlack,
    BrightRed,
    BrightGreen,
    BrightYellow,
    BrightBlue,
    BrightMagenta,
    BrightCyan,
    BrightWhite,

    None = 255
};
struct RGBOutColour {
    int8_t r = -1, g = -1, b = -1;
};


void setFOutColour(const std::string& text, OutputColour colour = OutputColour::None, RGBOutColour rgbColour = {}, bool resetAfter = false, unsigned int newLine = 0);
void setBOutColour(const std::string& text, OutputColour colour = OutputColour::None, RGBOutColour rgbColour = {}, bool resetAfter = false, unsigned int newLine = 0);
void setOutColour(const std::string& text, OutputColour foregroundColour = OutputColour::None, OutputColour backgroundColour = OutputColour::None, RGBOutColour foregroundRGBColour = {}, RGBOutColour backgroundRGBColour = {}, bool resetAfter = false, unsigned int newLine = 0);
void resetColour();


#endif //CTRACK_OUTPUT_H
