//
// Created by nyanm on 2022/4/20.
//

#include "aspParser.h"

#define COLL_MUSIC "music"
#define COLL_SKILL "skill"
#define COLL_PROFILE "profile"
#define COLL_PARAM "param"


aspParser::aspParser(const string &db_dir, int map_size, const string &card_num) {

    musicRecordMap = vector<musicRecord>(map_size * 5 + 1);
    fileLogger->debug("Set size of musicRecordMap to {}", map_size * 5 + 1);

    // initialize json parser
    Json::Reader reader;
    Json::Value root;
    // get raw database file
    ifstream raw_json;
    raw_json.open(db_dir, ios::in);
    fileLogger->debug("Open sdvx@asphyxia.db at [{}]", db_dir);
    string buffer;
    // initialize the latest time marks
    int64_t last_time, skill_time, profile_time, crew_time;
    last_time = skill_time = profile_time = crew_time = 0;

    while (getline(raw_json, buffer)) {
        try {
            // asphyxia the single line json dictionary
            reader.parse(buffer, root, false);
            // though some lines have no collection name, we can still keep them
            string collection = root["collection"].asString();

            // some lines have no refid or timestamp, pass them anyway
            string cur_id = root["__refid"].asString();
            int64_t cur_time = root["updatedAt"]["$$date"].asInt64();
            if (cur_id.empty() || !cur_time) continue;

            // specify user
            if (cur_id != card_num) continue;

            // music record, contains everything about this play
            if (collection == COLL_MUSIC) {
                int mid = root["mid"].asInt();
                int m_type = root["type"].asInt();

                if (!mid || !m_type) continue;
                int cur_index = mid * 5 + m_type;

                musicRecordMap[cur_index].isRecorded = true;
                musicRecordMap[cur_index].mid = mid;
                musicRecordMap[cur_index].musicType = m_type;
                musicRecordMap[cur_index].score = root["score"].asInt();
                musicRecordMap[cur_index].clear = root["clear"].asInt();
                musicRecordMap[cur_index].grade = root["grade"].asInt();
                musicRecordMap[cur_index].timeStamp = cur_time;
                if (root["exscore"].asInt()) musicRecordMap[cur_index].exScore = root["exscore"].asInt();

                if (cur_time > last_time) {
                    last_time = cur_time;
                    lastIndex = cur_index;
                }
            }

            // profile record, contains username, appeal card, aka name
            if (collection == COLL_PROFILE) {
                if (cur_time <= profile_time) continue;
                profile_time = cur_time;
                userName = root["name"].asString();
                apCard = root["appeal"].asInt();
                aka_index = root["akaname"].asInt();
            }

            // skill record, maintains the highest skill you've achieved
            if (collection == COLL_SKILL) {
                if (cur_time <= skill_time) continue;
                skill_time = cur_time;
                skill = max(root["base"].asInt(), skill);
            }

            // param record, use an uncanny way to store the crew
            if (collection == COLL_PARAM) {
                if (cur_time <= crew_time) continue;
                // specify crew param record
                if (root["type"].asInt() == 2 && root["id"] == 1) {
                    crew_time = cur_time;
                    crew_index = root["param"][24].asInt();
                }
            }
        } catch (exception &e) {
            fileLogger->error(e.what());
            fileLogger->error("The last json line is giving below:\n{}", buffer);
        }
    }
    fileLogger->debug("Parse sdvx@asphyxia.db complete");

    // no record found
    if (!lastIndex) {
        broadCastError("Music record not found, make sure you have at least played once (and saved successfully)");
        exit(1);
    }
    if (userName.empty()) {
        broadCastError("Profile record not found, make sure you have at least played once (and saved successfully)");
        exit(1);
    }

    // try to get crew id, otherwise the crewID will remain default
    auto crewIter = crew_id_map.find(crew_index);
    if (crewIter != crew_id_map.end()) crewID = crewIter->second;

    stringstream profile_log;
    profile_log << "Profile data loaded successfully:" << endl
                << "userName   :" << userName << endl
                << "appealCard :" << apCard << endl
                << "akaName    :" << akaName << endl
                << "skill      :" << skill << endl
                << "crewID     :" << crewID;
    broadCastInfo(profile_log.str());

    raw_json.close();
    fileLogger->flush();
}

void aspParser::update_akaname(const vector<akaData> &akaMap) {
    for (int index = 0; index < akaMap.size(); index += 1) {
        if (aka_index == akaMap[index].aID) {
            akaName = akaMap[index].name;
            fileLogger->debug("Update akaname to {}", akaName);
            return;
        }
    }
    fileLogger->info("Update akaname failed (no corresponding akaname index, maybe you are using a modified one)");
}