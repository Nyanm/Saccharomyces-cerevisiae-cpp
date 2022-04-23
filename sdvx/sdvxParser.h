//
// Created by nyanm on 2022/4/21.
//

#ifndef SDVX_CPP_SDVXPARSER_H
#define SDVX_CPP_SDVXPARSER_H

#include "fstream"
#include "string"
#include "iostream"
#include "sstream"
#include "tinyxml.h"
#include <codecvt>
#include <locale>
#include "regex"
#include "io.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include "../util/logger.h"

using namespace std;

const string db_name = "music.db";
const int metaVersion = 1;
const int fixVersion = 0;

struct levelData {
    int level = 0;
    string illustrator = "dummy";
    string effector = "dummy";
};

struct musicData {
    // info field
    int mID = 0;
    string name;
    string nameYmgn;  // ymgn = 読み仮名
    string artist;
    string artistYmgn;
    string mAscii;
    float bpmMax;
    float bpmMin;
    long date;
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
    int index;
    string akaname;
};

struct searchData {
    int mID;
    string meme;
};

class sdvxParser {

public:
    sdvxParser(const string &contents_dir);

    vector<musicData> musicMap;
    vector<akaData> akaMap;
    vector<searchData> searchMap;

    int mapSize = 0;

private:
    string ea3_path;
    string data_path;
    int64_t game_version = 0;

    SQLite::Database musicDB = SQLite::Database(db_name, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    bool update_check();

    void generate_database();

    void parse_music_db(string &xml_path);

    void parse_aka_db(string &xml_path);

    void update();

    string exist_meta = R"(SELECT count(*) FROM sqlite_master WHERE type= "table" AND name = "METADATA";)";
    string drop_meta = R"(DROP TABLE IF EXISTS METADATA;)";
    string set_meta = R"(
    CREATE TABLE METADATA(

    METADATA_VER INT PRIMARY KEY NOT NULL ,
    SDVX_VER INT NOT NULL ,
    FIX_VER INT NOT NULL,

    MAP_SIZE INT NOT NULL);)";

    string query_meta = R"(SELECT * FROM METADATA;)";

    vector<string> fix_before = {
            // latin field
            "驫", "騫", "曦", "頽", "罇", "曩", "齷", "彜", "骭", "鬥", "雋", "隍", "趁", "鬆", "驩",
            // symbol field
            "龕", "蹇", "鬻", "黻", "鑷",
            // graph field
            "齶", "齲", "躔", "釁", "齪", "鑈", "霻", "盥",
    };
    vector<string> fix_after = {
            // latin field
            "ā", "á", "à", "ä", "ê", "è", "é", "ū", "ü", "Ã", "Ǜ", "Ü", "Ǣ", "Ý", "Ø",
            // symbol field
            "€", "₂", "♃", "*", "ゔ",
            // graph field
            "♡", "❤", "★", "🍄", "♣", "♦", "♠", "⚙",
    };

    string fix_jis(const string &utf8_str);

    static string get_insert_meta(int64_t gameVer, int mapSize, int metaVer = metaVersion, int fixVer = fixVersion);

    static string jis_to_utf8(const char *jis_char);
};


#endif //SDVX_CPP_SDVXPARSER_H
