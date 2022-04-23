#include <iostream>
#include <windows.h>
#include "asphyxia/aspParser.h"
#include "sdvx/sdvxParser.h"
#include "util/logger.h"

using namespace std;

int main() {

    SetConsoleOutputCP(65001);

    sdvxParser sdvx("C:/Arcade MUG/SDVX6/KentuckyFriedChicken/contents");

    // aspParser asp(R"(C:\Arcade MUG\asphyxia-core\savedata\sdvx@asphyxia.db)",2000,"AB973E24894A6D58");

    return 0;
}
