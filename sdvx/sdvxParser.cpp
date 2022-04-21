//
// Created by nyanm on 2022/4/21.
//

#include "sdvxParser.h"

sdvxParser::sdvxParser() {
    // database validity check
    if (exist_db()) {

    } else generate();

}

void sdvxParser::generate() {
    broadCastInfo("Building music database.");
    SQLite::Transaction transaction(musicDB);
    // setup MUSIC

    // setup AKANAME

    // setup METADATA
    musicDB.exec(drop_meta);
    musicDB.exec(set_meta);

    transaction.commit();
}

bool sdvxParser::exist_db() {
    try {
        // search for METADATA
        SQLite::Statement exist_query(musicDB, exist_meta);
        int cnt = 0;
        while (exist_query.executeStep()) cnt = exist_query.getColumn(0);
        return cnt;
    } catch (exception &e) {
        // something unknown broke the program
        cout << e.what() << endl;
        return false;
    }
}

string sdvxParser::getInsertMeta(int gameVer, int metaVer, int fixVer){
    stringstream insert_meta;
    insert_meta << "INSERT INTO METADATA ()";
    return insert_meta.str();
}

bool versionCheck(){
    return true;
}
