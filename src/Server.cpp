#include "Server.h"
#include "URL.h"
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <iostream>

//для работы с HTTP 
namespace beast = boost::beast;  
namespace http = beast::http;
//Для работы с сетевыми операциями
namespace asio = boost::asio;
//Псевдоним для работы с TCP соединениями
using tcp = asio::ip::tcp;      

using namespace std;

//Конструктор с параметрами
Server::Server(const string& address, int port, const string& storage)
    : _address(address), _port(port), _url(storage) {}

void Server::run() {
    try {
        tcp::acceptor acceptor(_io_context, {asio::ip::make_address(_address), static_cast<unsigned short>(_port)});
        cout << "Сервер запущен на " << _address << ":" << _port << "\n";

        while (true) {
            tcp::socket socket(_io_context);
            acceptor.accept(socket);  //Ожидаем подключения
            cout << "Новое подключение принято" << endl;

            beast::flat_buffer buffer;
            http::request<http::string_body> req;

            try {
            //Чтение запроса
                http::read(socket, buffer, req);
                cout << "Получен запрос: " << req.method_string() << " " << req.target() << endl;
            } catch(const beast::system_error& e) {
        		cerr << "Ошибка чтения запроса: " << e.what() << '\n';
	        }

            http::response<http::string_body> res;

            //ОБработка запроса
            if (req.method() == http::verb::post && req.target() == "/shorten") {
                cout << "Обработка запроса на сокращение " << endl;
                res = _url.handle_shorten(req);
            } else {
                cout << "Обработка запроса на редирект " << endl;
                res = _url.handle_redirect(req);
            }
            try {
                //Отправка HTTP-ответа 
                http::write(socket, res);
                cout << "Ответ отправлен с кодом " << res.result_int() << endl;
            } catch(const beast::system_error& e) {
        		cerr << "Ошибка отправки ответа: " << e.what() << '\n';
	        }
        }
    } catch (const exception& e) {
        cerr << "Ошибка сервера: " << e.what() << "\n";
    }
}