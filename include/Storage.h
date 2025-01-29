#pragma once
#include <string>
#include <sqlite3.h>

using namespace std;

class Storage {
public:
    explicit Storage(const string& db_filename);
    ~Storage();
    
    void save(const string& short_url, const string& long_url);
    string get(const string& short_url);
    bool exists(const string& short_url);
private:
    sqlite3* _db = nullptr;
    void init_db();
};