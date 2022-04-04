#include <sys/types.h>//Содержит time_t для переносимости программы на другое устройство
#include <stdio.h> //стандартная библ ввода выв
#include <time.h> //для функц работы со врем
#include <stdlib.h>//для функц strlen
 
#define ERROR_PUTENV -1
#define ERROR_TIME -1
#define PUTENV_EXIT_ERROR 1
#define TIME_EXIT_ERROR 2
#define CTIME_EXIT_ERROR 3

int main() {
	time_t now;//Определяет переменную now типа time_t
	char *ctDate;

	int area = putenv("TZ=America/Los_Angeles");//Функция putenv добавляет новые переменные окружения или мо-
        						// дифицирует значения существующих переменных окружения

	if (area == ERROR_PUTENV){
		perror("Error putenv");
		exit(PUTENV_EXIT_ERROR);
	}

	int t = time(&now);//Вызывается time(2). Время в секундах от 00:00:00 UTC 1 января 1970 сохраняется в
				//переменной now

	if (t == ERROR_TIME){
            perror("Error time");
            exit(TIME_EXIT_ERROR);
    	}

	ctDate = ctime(&now);//Библиотечная функция ctime(3C) преобразует календарное время в ASCII-строку
				//формата date(1). Адрес, возвращенный этой функцией, используется в качестве параметра
				//printf для печати ASCII-строки

	if (ctDate == NULL){
            perror("Error ctime");
            exit(CTIME_EXIT_ERROR);
    	}

    	printf("%s", ctDate);

	return(0);
}
