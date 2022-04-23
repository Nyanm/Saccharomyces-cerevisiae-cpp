//
// Created by nyanm on 2022/4/23.
//

#ifndef SDVX_CPP_PJ_STRUCT_H
#define SDVX_CPP_PJ_STRUCT_H

#include "iostream"
#include "string"

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


struct levelData {
    int level = 0;
    std::string illustrator = "dummy";
    std::string effector = "dummy";
};

struct musicData {
    // info field
    int mID = 0;
    std::string name;
    std::string nameYmgn;  // ymgn = 読み仮名
    std::string artist;
    std::string artistYmgn;
    std::string mAscii;
    int bpmMax;
    int bpmMin;
    int date;
    int version;
    int infVer;
    // difficulty field
    levelData novice;
    levelData advanced;
    levelData exhaust;
    levelData infinite;
    levelData maximum;
};

struct akaData {
    int aID;
    std::string name;
};

struct searchData {
    int mID;
    std::string meme;
};

#endif //SDVX_CPP_PJ_STRUCT_H
