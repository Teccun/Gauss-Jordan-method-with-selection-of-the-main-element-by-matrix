#ifndef LSS_19_06_H
#define LSS_19_06_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define EPS 1e-8

// Вычисление необходимого размера для массива tmp
size_t lss_memsize_19_06(int n);

/* Коды возврата:
   0 - работа завершилась без ошибок, решение существует
   1 - работа завершилась без ошибок, решения не существует
  -1 - метод решения не применим к данной системе (система вырождена)
*/
int lss_19_06(int n, double* A, double* B, double* X, double* tmp);


#endif // LSS_19_06_H