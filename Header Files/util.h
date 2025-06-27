/* 
util.h

Declares utility functions for console manipulation, timestamp generation,
and other helper utilities.
*/

#pragma once

#include <string>
#include "screen.h"

std::string getCurrentTimestamp();
void printHeader();
void clearScreen();
void drawScreen(const ConsoleScreen& screen);