#include "lss_19_06.h"

size_t lss_memsize_19_06(int n) {
	if (n > 0)
		return (size_t)(2 * n) * sizeof(double);
	else
		return 0;
}

int lss_19_06(int n, double* A, double* B, double* X, double* tmp) {

	for (int i = 0; i < n; ++i) {
		tmp[i] = (double)i;
		tmp[n + i] = (double)i;
	}

	for (int k = 0; k < n; k++) {
		double best = 0.0;
		int best_i = -1, best_j = -1;
		for (int i = k; i < n; i++) {
			int row_i = (int)tmp[i];
			for (int j = k; j < n; j++) {
				int col_j = (int)tmp[n + j];
				double temp = fabs(A[row_i * n + col_j]);
				if (temp > best) {
					best = temp;
					best_i = i;
					best_j = j;
				}
			}
		}

		if (fabs(best) < EPS) {
			continue;
		}

		// swap элементов. 
		if (best_i != k) {
			double temp = tmp[k];
			tmp[k] = tmp[best_i];
			tmp[best_i] = temp;
		}
		if (best_j != k) {
			double temp = tmp[n + k];
			tmp[n + k] = tmp[n + best_j];
			tmp[n + best_j] = temp;
		}

		int rowk = (int)tmp[k];
		int colk = (int)tmp[n + k];
		double elem = A[rowk * n + colk];

		// нормализация
		for (int j = 0; j < n; j++) {
			int colj = (int)tmp[n + j];
			A[rowk * n + colj] /= elem;
		}
		B[rowk] /= elem;

		// зануление элементов столбца но строками ниже
		for (int i = 0; i < n; i++) {
			if (i == k)
				continue;
			int rowi = (int)tmp[i];
			double elem_below = A[rowi * n + colk];
			if (fabs(elem_below) > EPS) {
				for (int j = 0; j < n; j++) {
					int colj = (int)tmp[n + j];
					A[rowi * n + colj] -= elem_below * A[rowk * n + colj];
				}
				B[rowi] -= elem_below * B[rowk];
			}
		}
	}

	// проверка на несовместность
	for (int i = 0; i < n; i++) {
		double sumA = 0.0;
		for (int j = 0; j < n; j++) {
			sumA += fabs(A[i * n + j]);
		}
		if (sumA < EPS && fabs(B[i]) > EPS) {
			return 1;
		}
	}

	// расчитываем значения x 
	for (int k = 0; k < n; k++) {
		int rowk = (int)tmp[k];
		int colk = (int)tmp[n + k];
		X[colk] = B[rowk];
	}

	return 0;
}