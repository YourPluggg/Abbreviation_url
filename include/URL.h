#pragma once
#include "Storage.h"
#include <boost/beast/http.hpp>
#include <iostream>
#include <string>

namespace http = boost::beast::http;
using namespace std;

class URLHandler {
public:
    //Конструктор с одним параметром (для предотвращения неявного преобразования -> explicit)
    explicit URLHandler(const string& storage_file);
    string url_encode(const string& value);
    string generate_unique_short_url();
    //Обработка HTTP запроса на сокращение URL
    http::response<http::string_body> handle_shorten(const http::request<http::string_body>& req);
    //Обработка HTTP запроса на поиск исходного URL
    http::response<http::string_body> handle_redirect(const http::request<http::string_body>& req);
private:
    Storage _storage;
};