//
// Created by nyanm on 2022/4/21.
//

#include "sdvxParser.h"

sdvxParser::sdvxParser(const string &contents_dir) {
    // set paths
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
    game_version = _atoi64(ext->Value());
    fileLogger->debug("Sound Voltex game version {}", game_version);

    // database validity check
    if (update_check()) {
        // check versions in metadata
    };
}

void sdvxParser::update() {
    // parse music_db.xml
    string mdb_path = data_path + "/others/music_db.xml";
    parse_music_db(mdb_path);
    fileLogger->debug("Parse music_db.xml complete.");
    // parse akaname_parts.xml
    string aka_path = data_path + "/others/akaname_parts.xml";
    parse_aka_db(aka_path);
    fileLogger->debug("Parse akaname_parts.xml complete.");

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
    fileLogger->debug("Set map size to {}.", mapSize);
    fileLogger->flush();

    // setup musicMap and get map data
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
        cur_data->bpmMax = strtof(bpm_max->GetText(), nullptr);
        cur_data->bpmMin = strtof(bpm_min->GetText(), nullptr);
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

        // shift the index
        cur_music = cur_music->NextSiblingElement();
    } while (cur_music != last_music);
}

void sdvxParser::parse_aka_db(string &xml_path) {
    auto xml_db = new TiXmlDocument();
    xml_db->LoadFile(xml_path);
    TiXmlElement *akaname_part = xml_db->RootElement();
    fileLogger->debug("Open akaname_parts.xml at [{}]", xml_path);

    // parse and write akaname
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
    broadCastInfo("Building music database.");
    SQLite::Transaction transaction(musicDB);

    // setup METADATA
    if (!game_version || !mapSize) {
        broadCastError("Initialization is missing.");
        exit(1);
    }
    musicDB.exec(drop_meta);
    musicDB.exec(set_meta);
    musicDB.exec(get_insert_meta(game_version, mapSize));

    transaction.commit();
}

bool sdvxParser::update_check() {
    try {
        // search for METADATA
        SQLite::Statement exist_query(musicDB, exist_meta);
        int cnt = 0;
        while (exist_query.executeStep()) { cnt = exist_query.getColumn(0); }
        if (!cnt) return false;

        // check data in table METADATA
        SQLite::Statement meta_query(musicDB, query_meta);
        int db_meta_ver, db_game_ver, db_fix_ver;
        db_meta_ver = db_game_ver = db_fix_ver = 0;
        while (meta_query.executeStep()){
            db_meta_ver = meta_query.getColumn("METADATA_VER");
        }

        cout << db_meta_ver;

        return true;
    } catch (exception &e) {
        // something unknown broke the program
        cout << e.what() << endl;
        return false;
    }
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
