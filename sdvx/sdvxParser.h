//
// Created by nyanm on 2022/4/21.
//

#ifndef SDVX_CPP_SDVXPARSER_H
#define SDVX_CPP_SDVXPARSER_H

#include "fstream"
#include "string"
#include "iostream"
#include "sstream"
#include "SQLiteCpp/SQLiteCpp.h"
#include "../util/logger.h"

using namespace std;

const string db_name = "music.db";
const int metaVersion = 1;
const int fixVersion = 0;

class sdvxParser {

public:
    sdvxParser();


private:

    SQLite::Database musicDB = SQLite::Database(db_name, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    void generate();

    bool exist_db();

    string exist_meta = R"(SELECT count(*) FROM sqlite_master WHERE type= "table" AND name = "METADATA";)";
    string drop_meta = R"(DROP TABLE IF EXISTS METADATA;)";
    string set_meta = R"(
    CREATE TABLE METADATA(

    METADATA_VER INT PRIMARY KEY NOT NULL ,
    SDVX_VER INT NOT NULL ,
    FIX_VER INT NOT NULL,

    MAP_SIZE INT);)";

    string getInsertMeta(int gameVer, int metaVer = metaVersion,  int fixVer = fixVersion);
};


#endif //SDVX_CPP_SDVXPARSER_H
