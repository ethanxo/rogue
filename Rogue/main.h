#pragma once
#include <string>
#include <windows.h>

namespace guii {
    int menu();
    void boot();
}

namespace console {
    void prefixPrint(std::string text);
}