
#define _CRT_SECURE_NO_WARNINGS
#include "lss_19_06.h"

#define LSS_DEBUG        0x01
#define LSS_PRINT_ERROR  0x02
#define LSS_PRINT_MATRIX 0x04
#define LSS_TIME         0x08
#define LSS_FILEIN		 0x10
#define LSS_FILEOUT	     0x20


int read_from_file(const char* file_name, double** A, double** B, int* n, int print_error) {
	// Открытие файла и проверка на открытие
	FILE* f = fopen(file_name, "r");
	if (!f) {
		if (print_error) {
			printf("Error: cannot open input file '%s'\n", file_name);
		}
		return 100;
	}

	// Чтение размера матрицы и проверка на корректность
	if (fscanf(f, "%d", n) != 1) {
		if (print_error)
			printf("Error: Unable to read the number of equations\n");
		fclose(f);
		return 111;
	}
	else if (*n <= 0) {
		if (print_error)
			printf("Error: Matrix size must be positive, your size is '%d'\n", *n);
		fclose(f);
		return 112;
	}

	// Выделение памяти для матриц и векторов. Проверка на корректность
	*A = (double*)calloc((size_t)(*n) * (size_t)(*n), sizeof(double));
	*B = (double*)malloc((size_t)(*n) * sizeof(double));

	if (!*A) {
		if (print_error) {
			printf("Error: Memory allocation for A failed for n=%d\n", *n);
		}
		if (*A)
			free(*A);
		if (*B)
			free(*B);
		fclose(f);
		return 121;
	}
	if (!*B) {
		if (print_error) {
			printf("Error: Memory allocation for B failed for n=%d\n", *n);
		}
		if (*A)
			free(*A);
		if (*B)
			free(*B);
		fclose(f);
		return 122;
	}

	int count_first_row = 0;
	long pos = ftell(f);
	for (int k = 0; k < *n; k++) {
		if (fscanf(f, "%lf", &((*A)[k])) != 1) {
			if (print_error)
				printf("Error: Problem reading matrix A at position [%d][%d]\n", k, k);
			free(*A);
			free(*B);
			fclose(f);
			return 131;
		}
		count_first_row++;
		int c = getc(f);
		if (c == '\n' || c == EOF)
			break;
		ungetc(c, f);
	}

	fseek(f, pos, SEEK_SET);

	int is_band = 0;
	if (count_first_row == 2) {
		is_band = 1;
	}

	if (!is_band) {
		for (int i = 0; i < *n; i++) {
			for (int j = 0; j < *n; j++) {
				if (fscanf(f, "%lf", &((*A)[i * (*n) + j])) != 1) {
					printf("Debug: i=%d j=%d\n", i, j);
					if (print_error)
						printf("Error: Problem reading matrix A at position [%d][%d]\n", i, j);
					free(*A);
					free(*B);
					fclose(f);
					return 131;
				}
			}
		}
	}
	else {

		for (int i = 0; i < *n; i++) {

			int count = 0;
			double val;

			while (count < 3) {
				int res = fscanf(f, "%lf", &val);
				if (res == 1) {
					count++;
					if (i == 0) {
						if (count == 1) (*A)[i * (*n) + i] = val;
						else if (count == 2) {
							(*A)[i * (*n) + i + 1] = val;
							break;
						}
					}
					else if (i == *n - 1) {
						if (count == 1) (*A)[i * (*n) + i - 1] = val;
						else if (count == 2) { 
							(*A)[i * (*n) + i] = val; 
							break;
						}
					}
					else {
						if (count == 1) (*A)[i * (*n) + i - 1] = val;
						else if (count == 2) (*A)[i * (*n) + i] = val;
						else if (count == 3) (*A)[i * (*n) + i + 1] = val;
					}
				}
			}
		}
	}

	for (int i = 0; i < *n; i++) {
		if (fscanf(f, "%lf", &((*B)[i])) != 1) {
			printf("Debug: i=%d\n", i);
			if (print_error)
				printf("Error: Problem reading matrix B at position [%d]\n", i);
			free(*A);
			free(*B);
			fclose(f);
			return 132;
		}
	}

	fclose(f);
	return 0;
}

int write_to_file(const char* file_name, const double* X, int n, int print_error, int solution) {
	FILE* f = fopen(file_name, "w");
	if (!f) {
		if (print_error) {
			printf("Error: cannot open output file '%s'\n", file_name);
		}
		return 200;
	}

	if (solution == 0) {
		fprintf(f, "%d\n", n);
		for (int i = 0; i < n; i++) {
			fprintf(f, "%1.9lf\n", X[i]);
		}
	}
	else if (solution == 1) {
		fprintf(f, "0\n");
	}

	fclose(f);
	return 0;
}

void how_to_use_programm() {
	printf(
		"Usage: lss [input_file_name] [output_file_name] [options]\n \
		Where options include:\n \
		  -d        print debug messages [default OFF]\n \
		  -e        print errors [default OFF]\n \
		  -p        print matrix [default OFF]\n \
		  -t        print execution time [default OFF]\n \
		  -h, -?    print this and exit\n \
		Default input_file_name value is lss_19_06_in.txt,\n \
		default output_file_name value is lss_19_06_out.txt.\n");
}

void print_A_B_matrix(const double* A, const double* B, int n) {
	printf("Matrix A:\n");
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			printf("%lf ", A[i * n + j]);
		}
		printf("\n");
	}

	printf("Vector B:\n");
	for (int i = 0; i < n; i++) {
		printf("%1.9lf\n", B[i]);
	}
	printf("\n");
}

int main(int argc, char** argv) {
	// дефолтные имена файлов
	char in[256] = "lss_19_06_in.txt";
	char out[256] = "lss_19_06_out.txt";

		// наверное, в случае чего, можно вынести в отдельную фукнцию
	unsigned int flags = 0; // флаги опций программы, изначально тоключены
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == 'd' && argv[i][2] == '\0') {
				flags |= LSS_DEBUG;
			}
			else if (argv[i][1] == 'e' && argv[i][2] == '\0') {
				flags |= LSS_PRINT_ERROR;
			}
			else if (argv[i][1] == 'p' && argv[i][2] == '\0') {
				flags |= LSS_PRINT_MATRIX;
			}
			else if (argv[i][1] == 't' && argv[i][2] == '\0') {
				flags |= LSS_TIME;
			}
			else if ((argv[i][1] == 'h' || argv[i][1] == '?') && argv[i][2] == '\0') {
				how_to_use_programm();
				return 10;
			}
			else {
				printf("Error: Unknown option: %s\n", argv[i]);
				how_to_use_programm();
				return 11;
			}
		}
		else {
			// на первых двух позициях по идее должны быть имена файлов
			// но тогда я не смогу считать флаги, если хочу использовать базовые имена
			// можно было бы поднимать флаг при чтении
			if (!(flags & LSS_FILEIN)) {
				int count = 0;
				while ((in[count] = argv[i][count]) != '\0')
					count++;
				flags |= LSS_FILEIN;
			}
			else if (!(flags & LSS_FILEOUT)) {
				int count = 0;
				while ((out[count] = argv[i][count]) != '\0')
					count++;
				flags |= LSS_FILEOUT;
			}
			else {
				printf("Error: more than two file names entered\n");
				how_to_use_programm();
				return 12;
			}
		}
	}

	// Вывод отладочной информации. Этого достаточно?
	if (flags & LSS_DEBUG) {
		printf("[DEBUG] in='%s' out='%s' opts d=%d e=%d p=%d t=%d\n",
			in, out,
			(flags & LSS_DEBUG) ? 1 : 0,
			(flags & LSS_PRINT_ERROR) ? 1 : 0,
			(flags & LSS_PRINT_MATRIX) ? 1 : 0,
			(flags & LSS_TIME) ? 1 : 0);
	}

	// Чтение матриц из файла
	double* A = NULL;
	double* B = NULL;
	double* X = NULL;
	double* tmp = NULL;
	int n = 0;
	
	int read_error  = read_from_file(in, &A, &B, &n, (flags & LSS_PRINT_ERROR) ? 1 : 0);
	if (read_error != 0) {
		return read_error; // ошибка чтения
	}

	// Вывод матриц на экран, если поднят флаг
	if (flags & LSS_PRINT_MATRIX) {
		print_A_B_matrix(A, B, n);
	}

	X = (double*)malloc((size_t)n * sizeof(double));
	if (!X) {
		if (flags & LSS_PRINT_ERROR) {
			printf("Error: Memory allocation for X failed for n=%d\n", n);
		}
		if (A)
			free(A);
		if (B)
			free(B);
		return 123;
	}

	// для реализации потребуется небольшое количестве доп памяти
	size_t tmp_size = lss_memsize_19_06(n);
	if (tmp_size) {
		tmp = (double*)malloc(tmp_size);
		if (!tmp) {
			if (flags & LSS_PRINT_ERROR) {
				printf("Error: Memory allocation for tmp failed for n=%d\n", n);
			}
			if (A)
				free(A);
			if (B)
				free(B);
			if (X)
				free(X);
			return 124;
		}
	}

	clock_t start = clock();
	int solve = lss_19_06(n, A, B, X, tmp);
	clock_t end = clock();

	if (flags & LSS_TIME) {
		double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
		printf("Time spent for solving: %lf seconds\n", time_spent);
	}

	int write_error = 0;
	int result = 0;
	if (solve == 0) {
		write_error = write_to_file(out, X, n, (flags & LSS_PRINT_ERROR) ? 1 : 0, solve);
		if (write_error != 0) {
			result = write_error; 
		}
		else {
			result = 0; 
		}

		if (flags & LSS_DEBUG) {
			printf("Solution written to file '%s'\n", out);
		}
	}
	else if (solve == 1) {
		write_error = write_to_file(out, X, n, (flags & LSS_PRINT_ERROR) ? 1 : 0, solve);
		if (write_error != 0) {
			result = write_error; 
		}
		else {
			result = 1; 
		}

		if (flags & LSS_PRINT_ERROR) {
			printf("No solution exists for the given system.\n");
		}
	}
	else {
		if (flags & LSS_PRINT_ERROR) {
			printf("The method is not applicable to the given system (degenerate system).\n");
		}
		result = -1;
	}

	free(A);
	free(B);
	free(X);
	free(tmp);

	return result;
}