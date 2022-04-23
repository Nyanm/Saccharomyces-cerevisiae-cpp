//
// Created by nyanm on 2022/4/21.
//

#include "sdvxParser.h"

sdvxParser::sdvxParser(const string &contents_dir, bool forceUpdate) {
    // set paths and convertor
    ea3_path = contents_dir + "/prop/ea3-config.xml";
    data_path = contents_dir + "/data";

    fileLogger->debug("Generate ea3-config.xml path [{}]", ea3_path);
    fileLogger->debug("Generate ./contents/data path [{}]", data_path);
    // validity check
    if (_access(ea3_path.c_str(), 0) == -1 || _access(data_path.c_str(), 0) == -1) {
        broadCastError("Path of ea3-config or ./contents/data failed, "
                       "please check your file directory. Generated file paths were given in timber.log.");
        exit(1);
    }

    // get game version from ea3-config
    auto *ea3_config = new TiXmlDocument();
    ea3_config->LoadFile(ea3_path.c_str());
    TiXmlElement *ea3 = ea3_config->RootElement();
    TiXmlElement *soft = ea3->FirstChildElement("soft");
    TiXmlElement *ext = soft->FirstChildElement("ext");
    game_version = _atoi64(ext->GetText());
    fileLogger->debug("Sound Voltex game version {}", game_version);

    // database validity check
    if (forceUpdate) {
        fileLogger->info("Force update enabled");
        update();
    } else if (update_check()) { read_database(); } else update();  // update() will set the mapSize if it's ture

    broadCastInfo("SDVX data loaded");
    fileLogger->flush();
}


bool sdvxParser::update_check() {
    try {
        // search for METADATA
        SQLite::Statement exist_query(musicDB, exist_meta);
        int cnt = 0;
        while (exist_query.executeStep()) { cnt = exist_query.getColumn(0); }
        if (!cnt) return false;

        // search data in table METADATA
        SQLite::Statement meta_query(musicDB, query_meta);
        int64_t db_game_ver = 0;
        int db_meta_ver, db_fix_ver, db_map_size;
        db_meta_ver = db_fix_ver = db_map_size = 0;
        while (meta_query.executeStep()) {
            db_meta_ver = meta_query.getColumn("METADATA_VER").getInt();
            db_game_ver = meta_query.getColumn("SDVX_VER").getInt64();
            db_fix_ver = meta_query.getColumn("FIX_VER").getInt();
            db_map_size = meta_query.getColumn("MAP_SIZE").getInt();
        }
        fileLogger->debug("Metadata from database [METADATA_VER:{0}, SDVX_VER:{1}, FIX_VER:{2}, MAP_SIZE:{3}]",
                          db_meta_ver, db_game_ver, db_fix_ver, db_map_size);
        // check them
        if (db_meta_ver == META_VERSION && db_game_ver == game_version && db_fix_ver == FIX_VERSION) {
            broadCastInfo("Using latest database.");
            mapSize = db_map_size;  // if valid, set mapSize
            fileLogger->debug("Set map size to {}", mapSize);
            return true;
        } else return false;
    } catch (exception &e) {
        // something unknown broke the program
        fileLogger->error(e.what());
        return false;
    }
}

void sdvxParser::read_database() {
    try {
        // initialize attributes
        musicMap = vector<musicData>(mapSize);

        // read table MUSIC
        SQLite::Statement music_query(musicDB, query_music);
        int mid, bpm_max, bpm_min, date, version, inf_ver, nov_lv, adv_lv, exh_lv, inf_lv, mxm_lv;
        string name, name_yo, artist, artist_yo, music_ascii,
                nov_ill, nov_eff, adv_ill, adv_eff, exh_ill, exh_eff, inf_ill, inf_eff, mxm_ill, mxm_eff;
        while (music_query.executeStep()) {
            mid = music_query.getColumn("MID").getInt();
            musicData *record = &musicMap[mid];
            // info field
            record->mID = mid;
            record->name = music_query.getColumn("NAME").getString();
            record->nameYmgn = music_query.getColumn("NAME_YO").getString();
            record->artist = music_query.getColumn("ARTIST").getString();
            record->artistYmgn = music_query.getColumn("ARTIST_YO").getString();
            record->mAscii = music_query.getColumn("MUSIC_ASCII").getString();
            record->bpmMax = music_query.getColumn("BPM_MAX").getInt();
            record->bpmMin = music_query.getColumn("BPM_MIN").getInt();
            record->date = music_query.getColumn("DATE").getInt();
            record->version = music_query.getColumn("VERSION").getInt();
            record->infVer = music_query.getColumn("INF_VER").getInt();
            // difficulty field
            record->novice.level = music_query.getColumn("NOV_LV").getInt();
            record->novice.illustrator = music_query.getColumn("NOV_ILL").getString();
            record->novice.effector = music_query.getColumn("NOV_EFF").getString();
            record->advanced.level = music_query.getColumn("ADV_LV").getInt();
            record->advanced.illustrator = music_query.getColumn("ADV_ILL").getString();
            record->advanced.effector = music_query.getColumn("ADV_EFF").getString();
            record->exhaust.level = music_query.getColumn("EXH_LV").getInt();
            record->exhaust.illustrator = music_query.getColumn("EXH_ILL").getString();
            record->exhaust.effector = music_query.getColumn("EXH_EFF").getString();
            record->infinite.level = music_query.getColumn("INF_LV").getInt();
            record->infinite.illustrator = music_query.getColumn("INF_ILL").getString();
            record->infinite.effector = music_query.getColumn("INF_EFF").getString();
            record->maximum.level = music_query.getColumn("MXM_LV").getInt();
            record->maximum.illustrator = music_query.getColumn("MXM_ILL").getString();
            record->maximum.effector = music_query.getColumn("MXM_EFF").getString();
        }
        fileLogger->debug("Load musicRecordMap from database");

        // read table AKA
        SQLite::Statement aka_query(musicDB, query_aka);
        int aid;
        string a_name;
        while (aka_query.executeStep()) {
            aid = aka_query.getColumn("AID").getInt();
            a_name = aka_query.getColumn("NAME").getString();
            akaMap.insert(akaMap.end(), akaData{aid, a_name});
        }
        fileLogger->debug("Load akaMap from database");

        // read table SEARCH
        SQLite::Statement search_query(musicDB, query_search);

    } catch (exception &e) {
        fileLogger->error("Error occurs while querying the database, please try to reconstruct the database");
        fileLogger->error(e.what());
        exit(1);
    }
}

void sdvxParser::update() {
    // parse music_db.xml
    string mdb_path = data_path + "/others/music_db.xml";
    parse_music_db(mdb_path);
    fileLogger->debug("Parse music_db.xml complete");
    // parse akaname_parts.xml
    string aka_path = data_path + "/others/akaname_parts.xml";
    parse_aka_db(aka_path);
    fileLogger->debug("Parse akaname_parts.xml complete");

    // insert into sqlite database
    generate_database();
}

void sdvxParser::parse_music_db(string &xml_path) {
    // initialize xml file
    auto xml_db = new TiXmlDocument();
    xml_db->LoadFile(xml_path);
    TiXmlElement *mdb = xml_db->RootElement();
    fileLogger->debug("Open music_db.xml at [{}]", xml_path);

    // get map size
    TiXmlElement *size_music = mdb->FirstChildElement();
    TiXmlElement *last_music = mdb->LastChild("music")->ToElement();
    int tempMapSize = 0;
    do {
        int temp_mid = size_music->FirstAttribute()->IntValue();
        tempMapSize = max(tempMapSize, temp_mid);
        size_music = size_music->NextSiblingElement();
    } while (size_music != last_music);
    mapSize = tempMapSize + 1;
    fileLogger->debug("Set map size to {}", mapSize);
    fileLogger->flush();

    // setup musicRecordMap and get map data
    musicMap = vector<musicData>(mapSize);
    TiXmlElement *cur_music = mdb->FirstChildElement();

    // initialize pointers
    TiXmlElement *info, *label, *title_name, *title_yomigana, *artist_name, *artist_yomigana, *ascii, *bpm_max, *bpm_min, *distribution_date, *version, *inf_ver;
    TiXmlElement *difficulty, *cur_diff, *last_diff, *difnum, *illustrator, *effected_by;

    do {
        // initialize music ID
        int mid = cur_music->FirstAttribute()->IntValue();
        musicData *cur_data = &musicMap[mid];
        cur_data->mID = mid;

        // parse info part
        info = cur_music->FirstChildElement("info");  // head
        label = info->FirstChildElement("label");  // 1st children
        title_name = label->NextSiblingElement("title_name");
        title_yomigana = title_name->NextSiblingElement("title_yomigana");
        artist_name = title_yomigana->NextSiblingElement("artist_name");
        artist_yomigana = artist_name->NextSiblingElement("artist_yomigana");
        ascii = artist_yomigana->NextSiblingElement("ascii");
        bpm_max = ascii->NextSiblingElement("bpm_max");
        bpm_min = bpm_max->NextSiblingElement("bpm_min");
        distribution_date = bpm_min->NextSiblingElement("distribution_date");
        version = distribution_date->NextSiblingElement("version");
        inf_ver = version->NextSiblingElement("inf_ver");
        // write info part
        cur_data->name = fix_jis(jis_to_utf8(title_name->GetText()));
        cur_data->nameYmgn = jis_to_utf8(title_yomigana->GetText());
        cur_data->artist = fix_jis(jis_to_utf8(artist_name->GetText()));
        cur_data->artistYmgn = jis_to_utf8(artist_yomigana->GetText());
        cur_data->mAscii = ascii->GetText();
        cur_data->bpmMax = strtol(bpm_max->GetText(), nullptr, 10);
        cur_data->bpmMin = strtol(bpm_min->GetText(), nullptr, 10);
        cur_data->date = strtol(distribution_date->GetText(), nullptr, 10);
        cur_data->version = strtol(version->GetText(), nullptr, 10);
        cur_data->infVer = strtol(inf_ver->GetText(), nullptr, 10);

        // parse and write difficult part
        difficulty = info->NextSiblingElement("difficulty");  // head
        cur_diff = difficulty->FirstChildElement("novice");
        last_diff = difficulty->LastChild()->ToElement();
        for (int diff = 0; diff <= 4; diff += 1) {
            levelData *cur_level;
            // set difficulty
            if (diff == 0) cur_level = &cur_data->novice;
            else if (diff == 1) cur_level = &cur_data->advanced;
            else if (diff == 2) cur_level = &cur_data->exhaust;
            else if (diff == 3) cur_level = &cur_data->infinite;
            else if (diff == 4) cur_level = &cur_data->maximum;
            else break;
            // parse diff data
            difnum = cur_diff->FirstChildElement("difnum");
            illustrator = difnum->NextSiblingElement("illustrator");
            effected_by = illustrator->NextSiblingElement("effected_by");
            // write diff data
            cur_level->level = strtol(difnum->GetText(), nullptr, 10);
            cur_level->illustrator = fix_jis(jis_to_utf8(illustrator->GetText()));
            cur_level->effector = fix_jis(jis_to_utf8(effected_by->GetText()));
            // shift the difficulty
            if (cur_diff == last_diff) break;
            cur_diff = cur_diff->NextSiblingElement();
        }

        // shift the aID
        cur_music = cur_music->NextSiblingElement();
    } while (cur_music != last_music);
}

void sdvxParser::parse_aka_db(string &xml_path) {
    auto xml_db = new TiXmlDocument();
    xml_db->LoadFile(xml_path);
    TiXmlElement *akaname_part = xml_db->RootElement();
    fileLogger->debug("Open akaname_parts.xml at [{}]", xml_path);

    // parse and write name
    TiXmlElement *cur_part = akaname_part->FirstChildElement();
    TiXmlElement *last_part = akaname_part->LastChild()->ToElement();
    TiXmlElement *word;
    do {
        int aID = strtol(cur_part->FirstAttribute()->Value(), nullptr, 10);
        word = cur_part->FirstChildElement("word");
        string akaname = fix_jis(jis_to_utf8(word->GetText()));
        akaMap.insert(akaMap.end(), akaData{aID, akaname});
        cur_part = cur_part->NextSiblingElement();
    } while (cur_part != last_part);
}

void sdvxParser::generate_database() {
    broadCastInfo("Building music database");
    try {
        SQLite::Transaction transaction(musicDB);

        // setup METADATA
        if (!game_version || !mapSize) {
            broadCastError("Initialization is missing.");
            exit(1);
        }
        musicDB.exec(drop_meta);
        musicDB.exec(set_meta);
        musicDB.exec(get_insert_meta(game_version, mapSize, META_VERSION, FIX_VERSION));
        fileLogger->debug("Set table \"METADATA\"");

        // setup MUSIC
        musicDB.exec(drop_music);
        musicDB.exec(set_music);
        for (int index = 0; index < musicMap.size(); index += 1) {
            if (!musicMap[index].mID) continue;
            musicDB.exec(get_insert_music(musicMap[index]));
        }
        fileLogger->debug("Set table \"MUSIC\"");

        // setup AKA
        musicDB.exec(drop_aka);
        musicDB.exec(set_aka);
        for (int index = 0; index < akaMap.size(); index += 1) {
            if (!akaMap[index].aID) continue;
            musicDB.exec(get_insert_aka(akaMap[index]));
        }
        fileLogger->debug("Set table \"AKA\"");

        // setup SEARCH
        musicDB.exec(drop_search);
        musicDB.exec(set_search);
        fileLogger->debug("Table '\"SEARCH\" is on the way");

        transaction.commit();
        fileLogger->debug("Database ./music.db generated successfully");
    } catch (exception &e) {
        // something unknown broke the database down
        fileLogger->error(e.what());
        exit(1);
    }
}

string sdvxParser::jis_to_utf8(const char *jis_char) {
    if (!jis_char) return "";
    wstring_convert<codecvt_byname<wchar_t, char, mbstate_t>> convert(
            new codecvt_byname<wchar_t, char, mbstate_t>(".932"));
    wstring temp_str = convert.from_bytes(jis_char);
    wstring_convert<codecvt_utf8<wchar_t>> utf8_cvt;
    return utf8_cvt.to_bytes(temp_str);
}

string sdvxParser::fix_jis(const string &utf8_str) {
    if (utf8_str.empty()) return "";
    string fixed = utf8_str;
    for (int index = 0; index < fix_before.size(); index += 1) {
        fixed = regex_replace(fixed, regex(fix_before[index]), fix_after[index]);
    }
    return fixed;
}

string sdvxParser::fix_sql(const string &raw_str) {
    if (raw_str.empty()) return "";
    return regex_replace(raw_str, regex("'"), "''");
}

string sdvxParser::get_insert_meta(int64_t gameVer, int mapSize, int metaVer, int fixVer) {
    stringstream insert_meta;
    insert_meta << "INSERT INTO METADATA (METADATA_VER, SDVX_VER, FIX_VER, MAP_SIZE) VALUES ("
                << metaVer << ", "
                << gameVer << ", "
                << fixVer << ", "
                << mapSize << ");";
    return insert_meta.str();
}

string sdvxParser::get_insert_music(musicData &record) {
    stringstream insert_music;
    insert_music << "INSERT INTO MUSIC ("
                    "MID, NAME, NAME_YO, "
                    "ARTIST, ARTIST_YO, MUSIC_ASCII, "
                    "BPM_MAX, BPM_MIN, "
                    "DATE, VERSION, INF_VER, "
                    "NOV_LV, NOV_ILL, NOV_EFF, "
                    "ADV_LV, ADV_ILL, ADV_EFF, "
                    "EXH_LV, EXH_ILL, EXH_EFF, "
                    "INF_LV, INF_ILL, INF_EFF, "
                    "MXM_LV, MXM_ILL, MXM_EFF) VALUES ("
                 << record.mID << ", '"
                 << fix_sql(record.name) << "', '"
                 << fix_sql(record.nameYmgn) << "', '"
                 << fix_sql(record.artist) << "','"
                 << fix_sql(record.artistYmgn) << "', '"
                 << fix_sql(record.mAscii) << "', "
                 << record.bpmMax << ", "
                 << record.bpmMin << ", "
                 << record.date << ", "
                 << record.version << ", "
                 << record.infVer << ", "
                 << record.novice.level << ", '"
                 << fix_sql(record.novice.illustrator) << "', '"
                 << fix_sql(record.novice.effector) << "', "
                 << record.advanced.level << ", '"
                 << fix_sql(record.advanced.illustrator) << "', '"
                 << fix_sql(record.advanced.effector) << "', "
                 << record.exhaust.level << ", '"
                 << fix_sql(record.exhaust.illustrator) << "', '"
                 << fix_sql(record.exhaust.effector) << "', "
                 << record.infinite.level << ", '"
                 << fix_sql(record.infinite.illustrator) << "', '"
                 << fix_sql(record.infinite.effector) << "', "
                 << record.maximum.level << ", '"
                 << fix_sql(record.maximum.illustrator) << "', '"
                 << fix_sql(record.maximum.effector) << "');";
    return insert_music.str();
}

string sdvxParser::get_insert_aka(akaData &record) {
    stringstream insert_aka;
    insert_aka << "INSERT INTO AKA (AID, NAME) VALUES ("
               << record.aID << ", '"
               << fix_sql(record.name) << "');";
    return insert_aka.str();
}
