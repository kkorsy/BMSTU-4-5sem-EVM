#include <iostream>
#include <iterator>
#include <string>
#include <regex>
#include <sstream>
#include <fstream>
#include <ctime>
#include "host_main.h"

using namespace std;

#define CNT 254

/*
Вариант 10
Сетевой коммутатор на 128 портов. 
Сформировать в хост-подсистеме и передать в SPE таблицу коммутации из 254 ip адресов 195.19.32.1/24 (адреса 195.19.32.1 .. 195.19.32.254). 
Каждому адресу поставить в соответствие один из 128 интерфейсов (целые числа 0..127). 
Выполнить тестирование работы коммутатора, посылая из хост-подсистемы ip адреса и сравнивая полученный от GPC номер интерфейса с ожидаемым.
*/

int main(int argc, char** argv)
{
	ofstream log("task.log"); //поток вывода сообщений
	gpc *gpc64_inst; //указатель на класс gpc

	//Инициализация gpc
	if (argc < 2) {
		log << "Использование: host_main <путь к файлу rawbinary>" << endl;
		return -1;
	}

	//Захват ядра gpc и запись sw_kernel
	gpc64_inst = new gpc();
	log<<"Открывается доступ к "<<gpc64_inst->gpc_dev_path<<endl;
	if (gpc64_inst->load_swk(argv[1]) == 0) {
		log << "Программное ядро загружено из файла " << argv[1] << endl;
	}
	else {
		log << "Ошибка загрузки sw_kernel файла " << argv[1] << endl;
		return -1;
	}

	uint64_t correct[CNT];
	srand(time(0));

	gpc64_inst->start(__event__(update));
	for (uint64_t idx = 1; idx <= CNT; idx++) {
		gpc64_inst->mq_send(tables::key{.ip=idx});

		correct[idx - 1] = rand() % 128;
		gpc64_inst->mq_send(tables::val{.val=correct[idx - 1]});
	}
	gpc64_inst->mq_send(-1ull);
	
	// тестирование
	int passed = 0;
	gpc64_inst->start(__event__(select)); //обработчик запроса поиска 
	for (uint64_t idx = 1; idx <= CNT; ++idx)
	{
		log << "Запрошенный IP: 195.19.32." << idx << endl;
		gpc64_inst->mq_send(idx);

		uint64_t result = gpc64_inst->mq_receive();
    	if (result != -1) {
			log << "Найденный интерфейс: " << result << endl;
			if (result == correct[idx - 1]) {
				log << "\tТест " << idx << " пройден" << endl;
				passed++;
			}
			else
				log << "\tТест " << idx << " не пройден " << endl;
			log << endl;

		} else {
			log << "Интерфейс не найден." << endl;
		}
	}
	log << "Тестов пройдено: " << passed << "/" << CNT << endl;

	return 0;
}


