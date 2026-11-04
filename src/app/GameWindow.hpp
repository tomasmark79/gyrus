#pragma once
#include <string>

// Native SDL and Emscripten adapter around the portable game components.
int runGameWindow(unsigned frameLimit, const std::string& capture, bool preview);
