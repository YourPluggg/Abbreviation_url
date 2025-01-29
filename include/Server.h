#pragma once
#include <iostream>
#include "URL.h"
#include <boost/asio.hpp>
#include <string>

using namespace std;

class Server
{
private:
    string _address;
    int _port;
    URLHandler _url;
    boost::asio::io_context  _io_context;
public:
    Server(const string& address, int port, const string& storage);
    void run();
};
