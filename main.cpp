#include <iostream>
#include <windows.h>
#include "aspParser.h"

using namespace std;


int main() {
    SetConsoleOutputCP(65001);

    aspParser asp(R"(C:\Arcade MUG\asphyxia-core\savedata\sdvx@asphyxia.db)", 2000, "ABC");

    return 0;
}
