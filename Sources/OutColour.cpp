//
// Created by alex on 9/20/26.
//
#include "../Headers/OutColour.h"
#include <cstdint>
#include <iostream>
#include <string>

static bool isValidRGB(const RGBOutColour& rgb) {
    return (rgb.r != -1 && rgb.g != -1 && rgb.b != -1);
}

static std::string getAnsi(OutputColour colour) {
    auto value = static_cast<uint8_t>(colour);
    if (value < 8) {
        return "\033[" + std::to_string(30 + value) + 'm';
    }
    return "\033[" + std::to_string(90 + (value - 8)) + 'm';
}

static std::string getAnsiFromRgb(const RGBOutColour& rgb) {
    return "\033[38;2;" + std::to_string(rgb.r) + ';' + std::to_string(rgb.g) + ';' + std::to_string(rgb.b) + 'm';
}

static std::string getBgAnsi(OutputColour colour) {
    auto value = static_cast<uint8_t>(colour);
    if (value < 8) {
        return "\033[" + std::to_string(40 + value) + 'm';
    }
    return "\033[" + std::to_string(100 + (value - 8)) + 'm';
}

static std::string getAnsiBgFromRgb(const RGBOutColour& rgb) {
    return "\033[48;2;" + std::to_string(rgb.r) + ';' + std::to_string(rgb.g) + ';' + std::to_string(rgb.b) + 'm';
}

static void outNewLines(unsigned int newLines = 0) {
    if (newLines > 0) {
        std::cout << std::string(newLines, '\n');
    }
}

void setFOutColour(const std::string& text, const OutputColour colour, const RGBOutColour rgbColour, bool resetAfter, unsigned int newLine) {
    std::string ansiColour;
    if (colour != OutputColour::None) {
        ansiColour = getAnsi(colour);
    } else if (isValidRGB(rgbColour)) {
        ansiColour = getAnsiFromRgb(rgbColour);
    }

    std::cout << ansiColour << text;
    if (resetAfter) resetColour();
    outNewLines(newLine);
    std::cin.clear();
}

void setBOutColour(const std::string& text, OutputColour colour, RGBOutColour rgbColour, bool resetAfter, unsigned int newLine) {
    std::string ansiColour;
    if (colour != OutputColour::None) {
        ansiColour = getBgAnsi(colour);
    } else if (isValidRGB(rgbColour)) {
        ansiColour = getAnsiBgFromRgb(rgbColour);
    }

    std::cout << ansiColour << text;
    if (resetAfter) resetColour();
    outNewLines(newLine);
}

void setOutColour(const std::string& text, OutputColour foregroundColour, OutputColour backgroundColour, RGBOutColour foregroundRGBColour, RGBOutColour backgroundRGBColour, bool resetAfter, unsigned int newLine) {
    std::string foregroundAnsi = "";
    std::string backgroundAnsi = "";

    if (foregroundColour != OutputColour::None) {
        foregroundAnsi = getAnsi(foregroundColour);
    } else if (isValidRGB(foregroundRGBColour)) {
        foregroundAnsi = getAnsiFromRgb(foregroundRGBColour);
    }

    if (backgroundColour != OutputColour::None) {
        backgroundAnsi = getBgAnsi(backgroundColour);
    } else if (isValidRGB(backgroundRGBColour)) {
        backgroundAnsi = getAnsiBgFromRgb(backgroundRGBColour);
    }

    std::cout << foregroundAnsi << backgroundAnsi << text;
    if (resetAfter) resetColour();
    outNewLines(newLine);
}

void resetColour() {
    std::cout << "\033[0m";
}