//
// Created by nyanm on 2022/4/20.
//

#ifndef SDVX_CPP_ASPPARSER_H
#define SDVX_CPP_ASPPARSER_H

#include "vector"
#include "fstream"
#include "sstream"
#include "json/json.h"
#include "map"

#include "../util/logger.h"
#include "../util/pj_struct.h"

using namespace std;

class aspParser {

public:
    aspParser(const string &db_dir, int map_size, const string &card_num);

    void update_akaname(const vector<akaData>& akaMap);

    vector<musicRecord> musicRecordMap;
    int lastIndex = 0;

    // user profile
    string userName;
    int apCard = 6001;  // gen6 default appeal card
    string akaName = "よろしくお願いします"; // default name id=1
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
