//
// Created by nyanm on 2022/4/20.
//

#ifndef SDVX_CPP_ASPPARSER_H
#define SDVX_CPP_ASPPARSER_H

#include "vector"
#include "string"
#include "json/json.h"

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
    int last_index = 0;

    // user profile


};


#endif //SDVX_CPP_ASPPARSER_H
