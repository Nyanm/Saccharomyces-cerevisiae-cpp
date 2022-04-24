#include <windows.h>
#include "asphyxia/aspParser.h"
#include "sdvx/sdvxParser.h"


using namespace std;

int main() {

    SetConsoleOutputCP(65001);

    sdvxParser sdvx("C:/Arcade MUG/SDVX6/KentuckyFriedChicken/contents", false);

    aspParser asp(R"(C:\Arcade MUG\asphyxia-core\savedata\sdvx@asphyxia.db)", sdvx.mapSize, "AB973E24894A6D58");
    asp.updateAkaName(sdvx.akaMap);
    asp.updateVolForce(sdvx.musicMap);

    return 0;
}
