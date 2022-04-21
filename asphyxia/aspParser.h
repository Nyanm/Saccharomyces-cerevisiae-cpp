//
// Created by nyanm on 2022/4/20.
//

#ifndef SDVX_CPP_ASPPARSER_H
#define SDVX_CPP_ASPPARSER_H

#include "vector"
#include "string"
#include "fstream"
#include "iostream"
#include "sstream"
#include "json/json.h"
#include "map"
#include "../util/logger.h"

using namespace std;

struct musicRecord {
    bool isRecorded = false;
    int mid = 0;
    int musicType = 0;
    int score = 0;
    int clear = 0;
    int grade = 0;
    int64_t timeStamp = 0;
    int exScore = 0;

    int level = 0;
    float volForce = 0.0;
};

class aspParser {

public:
    aspParser(const string &db_dir, int map_size, const string &card_num); // __init__
    vector<musicRecord> musicMap;
    int lastIndex = 0;

    // user profile
    string userName;
    int apCard = 6001;  // gen6 default appeal card
    string akaName = "よろしくお願いします"; // default akaname id=1
    int skill = 0;  // no rank
    string crewID = "0014";  // Gen 6 Rasis

private:
    int crew_index = 0;
    int aka_index = 0;

    const map<int, string> crew_id_map = {
            {116, "0001"}
    };

};


#endif //SDVX_CPP_ASPPARSER_H
