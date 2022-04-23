//
// Created by nyanm on 2022/4/21.
//

#ifndef SDVX_CPP_SDVXPARSER_H
#define SDVX_CPP_SDVXPARSER_H

#include "fstream"
#include "sstream"
#include "tinyxml.h"
#include <codecvt>
#include <locale>
#include "regex"
#include "io.h"
#include "SQLiteCpp/SQLiteCpp.h"

#include "../util/logger.h"
#include "../util/pj_cfg.h"
#include "../util/pj_struct.h"

using namespace std;

const string db_name = "music.db";


class sdvxParser {

public:
    sdvxParser(const string &contentsDir, bool forceUpdate);

    vector<musicData> musicMap;
    vector<akaData> akaMap;
    vector<searchData> searchMap;

    int mapSize = 0;

private:
    // initialized in constructor
    string ea3_path;
    string data_path;
    // initialized in update_check()
    int64_t game_version = 0;
    // read it or create an empty one
    SQLite::Database musicDB = SQLite::Database(db_name, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    // update check
    bool update_check();

    // update check handler
    string query_meta = R"(SELECT * FROM METADATA;)";

    // update_check() == true
    void read_database();  // set musicRecordMap, akaMap
    // read handlers
    string query_music = R"(SELECT * FROM MUSIC;)";
    string query_aka = R"(SELECT * FROM AKA;)";
    string query_search = R"(SELECT * FROM SEARCH;)";

    // update_check() == false
    void update();

    void parse_music_db(string &xml_path);  // set musicRecordMap
    void parse_aka_db(string &xml_path);  // set akaMap

    // generate handlers
    void generate_database();

    // METADATA handler
    string exist_meta = R"(SELECT count(*) FROM sqlite_master WHERE type= "table" AND name = "METADATA";)";
    string drop_meta = R"(DROP TABLE IF EXISTS METADATA;)";
    string set_meta = R"(
    CREATE TABLE METADATA(

    METADATA_VER INT PRIMARY KEY NOT NULL ,
    SDVX_VER INT NOT NULL ,
    FIX_VER INT NOT NULL ,

    MAP_SIZE INT NOT NULL);)";

    static string get_insert_meta(int64_t gameVer, int mapSize, int metaVer, int fixVer);

    // MUSIC handler
    string drop_music = R"(DROP TABLE IF EXISTS MUSIC;)";
    string set_music = R"(
    CREATE TABLE MUSIC(

    MID INT PRIMARY KEY NOT NULL ,
    NAME VARCHAR NOT NULL ,
    NAME_YO VARCHAR NOT NULL ,
    ARTIST VARCHAR NOT NULL ,
    ARTIST_YO VARCHAR NOT NULL ,
    MUSIC_ASCII VARCHAR NOT NULL ,
    BPM_MAX INT NOT NULL ,
    BPM_MIN INT NOT NULL ,
    DATE INT NOT NULL ,
    VERSION INT NOT NULL ,
    INF_VER INT ,

    NOV_LV INT ,
    NOV_ILL VARCHAR ,
    NOV_EFF VARCHAR ,
    ADV_LV INT ,
    ADV_ILL VARCHAR ,
    ADV_EFF VARCHAR ,
    EXH_LV INT ,
    EXH_ILL VARCHAR ,
    EXH_EFF VARCHAR ,
    INF_LV INT ,
    INF_ILL VARCHAR ,
    INF_EFF VARCHAR ,
    MXM_LV INT ,
    MXM_ILL VARCHAR ,
    MXM_EFF VARCHAR);)";

    static string get_insert_music(musicData &record);

    // AKANAME handler
    string drop_aka = R"(DROP TABLE IF EXISTS AKA;)";
    string set_aka = R"(CREATE TABLE AKA (AID INT PRIMARY KEY NOT NULL, NAME VARCHAR);)";

    static string get_insert_aka(akaData &record);

    // SEARCH handler
    string drop_search = R"(DROP TABLE IF EXISTS SEARCH;)";
    string set_search = R"(CREATE TABLE SEARCH (MID INT PRIMARY KEY NOT NULL, MEME VARCHAR);)";

    // string utilities
    vector<string> fix_before = {
            // latin field
            "驫", "騫", "曦", "頽", "罇",
            "曩", "齷", "彜", "骭", "鬥",
            "雋", "隍", "趁", "鬆", "驩",
            // symbol field
            "龕", "蹇", "鬻", "黻", "鑷",
            // graph field
            "齶", "齲", "躔", "釁", "齪",
            "鑈", "霻", "盥",
    };
    vector<string> fix_after = {
            // latin field
            "ā", "á", "à", "ä", "ê",
            "è", "é", "ū", "ü", "Ã",
            "Ǜ", "Ü", "Ǣ", "Ý", "Ø",
            // symbol field
            "€", "₂", "♃", "*", "ゔ",
            // graph field
            "♡", "❤", "★", "🍄", "♣",
            "♦", "♠", "⚙",
    };

    string fix_jis(const string &utf8_str);

    static string fix_sql(const string &raw_str);

    static string jis_to_utf8(const char *jis_char);

};

#endif //SDVX_CPP_SDVXPARSER_H
