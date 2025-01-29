#include "Server.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[]) {
	//Проверяем количество аргументов командой строки
	if (argc != 4)
	{
		cerr <<  "Ииспользование: " << argv[0] << " адрес, порт, путь к файлу " << endl;
		return 1;
	}
	
	//Получаем параметры из аргументов командой строки
	string address = argv[1];
	int port = stoi(argv[2]);
	string storage =argv[3];

	try
	{
		Server server(address, port, storage);
		server.run();
	}
	catch(const exception& e)
	{
		cerr << "Ошибка: " << e.what() << '\n';
		return 1;
	}

	return 0;
}
