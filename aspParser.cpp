//
// Created by nyanm on 2022/4/20.
//

#include "aspParser.h"
#include "iostream"
#include "fstream"

#define COLL_MUSIC "music"
#define COLL_SKILL "skill"
#define COLL_PROFILE "profile"
#define COLL_PARAM "param"

aspParser::aspParser(const string &db_dir, int map_size, const string &card_num) {

    musicMap = vector<musicRecord>(map_size * 5 + 1);

    Json::Reader reader;
    Json::Value root;

    // get raw database file
    ifstream raw_json;
    raw_json.open(db_dir, ios::in);
    string buffer;
    int64_t last_time = 0;

    while (getline(raw_json, buffer)) {

        reader.parse(buffer, root, false);
        string collection = root["collection"].asString();
        string cur_card = root["__refid"].asString();
        int64_t cur_time = root["updatedAt"]["$$date"].asInt64();

        if (collection == COLL_MUSIC) {
            int mid = root["mid"].asInt();
            int m_type = root["type"].asInt();

            if (!mid || !m_type) continue;
            int cur_index = mid * 5 + m_type;

            musicMap[cur_index].isRecorded = true;
            musicMap[cur_index].mid = mid;
            musicMap[cur_index].musicType = m_type;
            musicMap[cur_index].score = root["score"].asInt();
            musicMap[cur_index].clear = root["clear"].asInt();
            musicMap[cur_index].grade = root["grade"].asInt();
            musicMap[cur_index].timeStamp = cur_time;
            if (root["exscore"].asInt()) musicMap[cur_index].exScore = root["exscore"].asInt();

            if (cur_time > last_time) {
                last_time = cur_time;
                last_index = cur_index;
            }

        } else if (collection == COLL_PROFILE) {

        } else if (collection == COLL_SKILL) {

        } else if (collection == COLL_PARAM) {

        }
    }
    raw_json.close();
}
