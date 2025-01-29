#include "URL.h"
#include <boost/beast/http.hpp>
#include <random>
#include <iostream>
#include <iomanip>
#include <codecvt>
#include <locale>

//для работы с HTTP 
namespace http = boost::beast::http;

using namespace std;

// Конструктор с параметрами
URLHandler::URLHandler(const string& db_file) : _storage(db_file) {}

// Функция для кодирования русских символов в URL (percent-encoding)
string URLHandler::url_encode(const string& value) {
    ostringstream escaped;
    escaped.fill('0');
    escaped << hex;

    for (char c : value) {
        if (isalnum(static_cast<unsigned char>(c)) || 
            c == '-' || c == '_' || c == '.' || c == '~' || 
            c == ':' || c == '/' || c == '?' || c == '&' || c == '=') {
            escaped << c;
        } else {
            escaped << '%' << setw(2) << uppercase << int(static_cast<unsigned char>(c));
        }
    }
    return escaped.str();
}

// Функция для генерации уникального короткого кода (6 случайных символов) с проверкой коллизий
string URLHandler::generate_unique_short_url() {
    static const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, sizeof(chars) - 2);

    string short_url;
    do {
        short_url.clear();
        for (int i = 0; i < 6; ++i) {
            short_url += chars[distrib(gen)];
        }
    } while (_storage.exists(short_url)); // Проверяем наличие в базе

    cout << "Сгенерирован уникальный короткий адрес: " << short_url << endl;
    return short_url;
}

// Обработка запроса на сокращение URL
http::response<http::string_body> URLHandler::handle_shorten(const http::request<http::string_body>& req) {
    string long_url = req.body(); // Извлекаем URL из тела запроса
    cout << "Получен запрос на сокращение URL: " << long_url << endl;
    
    // Кодируем русские символы
    string encoded_url = url_encode(long_url);
    
    string short_url = generate_unique_short_url();
    _storage.save(short_url, encoded_url);
    cout << "URL сохранен: " << short_url << " -> " << encoded_url << endl;

    http::response<http::string_body> res(http::status::ok, req.version());
    res.set(http::field::content_type, "application/json");
    res.body() = "{\"short_url\": \"" + short_url + "\"}";
    res.prepare_payload();
    
    return res;
}

// Обработка запроса на перенаправление
http::response<http::string_body> URLHandler::handle_redirect(const http::request<http::string_body>& req) {
    string short_url = string(req.target()).substr(1); // Извлекаем короткий URL
    cout << "Запрос на редирект: " << short_url << endl;
    
    string long_url = _storage.get(short_url);

    if (long_url.empty()) {
        // Если не нашли
        cout << "Короткий URL не найден в базе: " << short_url << endl;
        http::response<http::string_body> res(http::status::not_found, req.version());
        res.set(http::field::content_type, "text/plain");
        res.body() = "URL not found";
        res.prepare_payload();
        return res;
    }
    
    // Формирование HTTP ответа с редиректом
    cout << "Перенаправление: " << short_url << " -> " << long_url << endl;
    http::response<http::string_body> res(http::status::moved_permanently, req.version());
    res.set(http::field::location, long_url);
    res.set(http::field::content_type, "text/plain");
    res.body() = "Перенаправление на " + long_url;
    res.prepare_payload();
    
    return res;
}