#include "Storage.h"
#include <iostream>

using namespace std;

// Конструктор с параметрами
Storage::Storage(const string& db_filename) {
    cout << "Открываем базу данных: " << db_filename << endl;

    if (sqlite3_open(db_filename.c_str(), &_db) != SQLITE_OK) {
        cerr << "Ошибка открытия БД: " << sqlite3_errmsg(_db) << endl;
    } else {
        cout << "База данных успешно открыта." << endl;
        init_db();
    }
}

// Деструктор 
Storage::~Storage() {
    cout << "Закрываем базу данных." << endl;
    sqlite3_close(_db);
}

//Инициализация БД
void Storage::init_db() {
    const char* create_table_sql =
        "CREATE TABLE IF NOT EXISTS urls ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "short_url TEXT UNIQUE, "
        "long_url TEXT);";

    char* err_msg = nullptr;
    // Выполнение запроса для создания 
    if (sqlite3_exec(_db, create_table_sql, 0, 0, &err_msg) != SQLITE_OK) {
        cerr << "Ошибка создания таблицы: " << err_msg << endl;
        sqlite3_free(err_msg);  
    } else {
        cout << "Таблица создана или уже существует." << endl;
    }
}

// Сохраняем пару [короткий, исходный] URL в БД
void Storage::save(const string& short_url, const string& long_url) {
    const char* insert_sql = "INSERT INTO urls (short_url, long_url) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    cout << "Вставка данных в таблицу: короткий URL = " << short_url << ", длинный URL = " << long_url << endl;

    // Вставка данных
    if (sqlite3_prepare_v2(_db, insert_sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, short_url.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, long_url.c_str(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            cerr << "Ошибка при вставке данных: " << sqlite3_errmsg(_db) << endl;
        } else {
            cout << "Данные успешно вставлены в базу." << endl;
        }
    } else {
        cerr << "Ошибка запроса на вставку: " << sqlite3_errmsg(_db) << endl;
    }

    sqlite3_finalize(stmt);
}

// Получаем исходный URL по короткому
string Storage::get(const string& short_url) {
    string long_url;
    const char* select_sql = "SELECT long_url FROM urls WHERE short_url = ?;";
    sqlite3_stmt* stmt;

    // Ищем исходный URL
    if (sqlite3_prepare_v2(_db, select_sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, short_url.c_str(), -1, SQLITE_STATIC);
        
        // Выполнение запроса 
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            long_url = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            cout << "Найден исходный URL: " << long_url << endl;
        } else {
            cout << "Исходный URL не найден для короткого: " << short_url << endl;
        }
    } else {
        cerr << "Ошибка запроса на поиск: " << sqlite3_errmsg(_db) << endl;
    }

    sqlite3_finalize(stmt);
    return long_url;  
}

//Проверка на существование такого URL
bool Storage::exists(const string& short_url) {
    const char* select_sql = "SELECT 1 FROM urls WHERE short_url = ? LIMIT 1;";
    sqlite3_stmt* stmt;
    bool found = false;

    if (sqlite3_prepare_v2(_db, select_sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, short_url.c_str(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            found = true;
        }
    } else {
        cerr << "Ошибка проверки существования URL: " << sqlite3_errmsg(_db) << endl;
    }

    sqlite3_finalize(stmt);
    return found;
}