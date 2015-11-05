/*
 ============================================================================
 Name        : lab.c
 Author      : Julio Vásquez
 Version     : 0.1
 Copyright   : Todo lo que figure en este codigo puede ser copiado y expuesto,
 siempre y cuando se señale el autor de este, o sea Julio Vásquez
 Description : Laboratorio 1 en C Paradigmas de la Programación
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

FILE *f;

typedef struct {
	int valor;
	int lock;
} casilla;

typedef struct {
	casilla **matriz;
	int largo;
	int ancho;
	int id;
} board;

typedef struct {
	board *tablero;
	int id_player;
	float avanceParcial;
	int date;
	int time;

} game;

typedef enum code {
	OK, NO_MEMORY, WRONG_DIM, ERR_FILE_NOT_FOUND, ERR_FILE_NOT_PERMISSION, FAIL
} code;

void saveBoard(board *b, int *id) {
	int x, y;
	int v1 = 0;
	int v2 = 0;
	int N = b->largo;
	time_t current_time;
	struct tm * time_info;
	char timeString[12];  // space for "HH:MM:SS\0"
	char timeString2[30];
	time(&current_time);
	time_info = localtime(&current_time);
	strftime(timeString, sizeof(timeString), "%m%d%H%M%S", time_info);
	strftime(timeString2, sizeof(timeString2), "%B %d %H:%M:%S", time_info);
	f = fopen(timeString, "w");
	fprintf(f, "%d\n", N);
	for (x = 0; x < N; x++) {
		for (y = 0; y < N; y++) {
			fprintf(f, "%d ", b->matriz[x][y].valor);
			v1 = v1 + b->matriz[x][y].valor;
		}
		fprintf(f, "\n");
	}
	fprintf(f, "%d\n", v1);
	for (x = 0; x < N; x++) {
		for (y = 0; y < N; y++) {
			fprintf(f, "%d ", b->matriz[x][y].lock);
			v2 = v2 + b->matriz[x][y].lock;
		}
		fprintf(f, "\n");
	}

	fprintf(f, "%d\n", v2);
	fprintf(f, "%s", timeString2);
	int algo = atoi(timeString);
	*id = algo;
	//*a = algo
	//*id = *a;
	b->id = algo;
	printf("ALGO: %d // ID: %d\n", algo, *id);
	fclose(f);
}

int verificador(board tablero, int num, int x, int y, int N, int visibility) {
	int i, j, v2, v1, v3, v4;
	for (i = 0; i < N; i++)
		if (i != x)
			if (num == tablero.matriz[i][y].valor) {
				if (visibility != 0)
					printf("Se encontró otro %d en la columna\n", num);
				return 1;
			}
	for (j = 0; j < N; j++)
		if (j != y)
			if (num == tablero.matriz[x][j].valor) {
				if (visibility != 0)
					printf("Se encontró otro %d en la fila\n", num);
				return 1;
			}
	v1 = 0;
	v2 = sqrt(N) - 1;
	v3 = 0;
	v4 = sqrt(N) - 1;
	while (x > v2 && x > v1) {
		v2 = v2 + sqrt(N);
		v1 = v1 + sqrt(N);
	}
	while (y > v4 && y > v3) {
		v4 = v4 + sqrt(N);
		v3 = v3 + sqrt(N);
	}
	for (i = v1; i <= v2; i++) {
		for (j = v3; j <= v4; j++) {
			if (i != x && j != y)
				if (num == tablero.matriz[i][j].valor) {
					if (visibility == 1)
						printf("Se encontró otro %d en la región interior\n",
								num);
					return 1;
				}
		}
	}

	return 0;
}

int rules(board *b) {
	int x, y;
	for (x = 0; x < b->ancho; x++) {
		for (y = 0; y < b->ancho; y++) {
			//printf("valor %d\n", b->matriz[x][y].valor);
			if (b->matriz[x][y].valor != 0
					&& verificador(*b, b->matriz[x][y].valor, x, y, b->ancho, 0)
							!= 0) {
				printf("Violación de reglas\n");
				return 1;
			}
		}
	}
//	for (x = 0; x < b->ancho; x++) {
//		for (y = 0; y < b->ancho; y++) {
//			if (b->matriz[x][y].valor == 0) {
//				for (i = 1; i <= b->ancho; i++) {
//					if (b->matriz[x][y].valor == 0 && verificador(*b, b->matriz[x][y].valor, x, y, b->ancho,
//							0) == 0) {
//						i = b->ancho+1;
//					}
//					else{
//						b->matriz[x][y].valor = i;
//					}
//				}
//				printf("Tablero sin solución!!!\n\n");
//			}
//		}
//	}
	return 0;
}

board* loadBoard(int id) {
	char str[15];
	sprintf(str, "%d", id);
	f = fopen(str, "r");
	if (f == NULL) {
		printf("No se encontró archivo\n");
		return NULL;
	}
	board* tablero = malloc(sizeof(tablero));
	casilla **aux;
	int N, x, y, verif, acum;
	fscanf(f, "%d\n", &N);
	aux = malloc(N * sizeof(casilla*));
	for (x = 0; x < N; x++) {
		aux[x] = malloc(N * sizeof(casilla));
	}
	acum = 0;
	for (x = 0; x < N; x++) {
		for (y = 0; y < N; y++) {
			fscanf(f, "%d", &aux[x][y].valor);
			acum = acum + aux[x][y].valor;
			//printf("Se extrajo %d\n", aux[x][y].valor);
		}

		fscanf(f, "\n");
	}
	fscanf(f, "%d\n", &verif); //verificador, despues lo vere
	if (verif != acum) {
		printf("Tablero corrompido o manipulado por un tercero.\n");
		return NULL;
	}
	acum = 0;
	for (x = 0; x < N; x++) {
		for (y = 0; y < N; y++) {
			fscanf(f, "%d", &aux[x][y].lock);
			acum = acum + aux[x][y].lock;
			//printf("Se extrajo %d\n", aux[x][y].lock);
		}
		fscanf(f, "\n");
	}
	fscanf(f, "%d", &verif); //verificador, despues lo vere
	if (verif != acum) {
		printf("Tablero corrompido o manipulado por un tercero.\n");
		return NULL;
	}
	tablero->matriz = aux;
	tablero->largo = N;
	tablero->ancho = N;
	fclose(f);
	return tablero;
}

board* createBoard(int N, int M, int difficulty) {
	int x, y, z, w, max, cont, cont1;
	int visibility = 0;
	casilla **aux;
	time_t t;
	srand((unsigned) time(&t));
	board* tablero = malloc(sizeof(tablero));
	aux = malloc(N * sizeof(casilla*));
	for (x = 0; x < N; x++) {
		aux[x] = malloc(N * sizeof(casilla));
	}
	for (x = 0; x < N; x++) {
		for (y = 0; y < N; y++) {
			aux[x][y].valor = 0;
			aux[x][y].lock = 0;
		}
	}
	switch (difficulty) {
	case 1:
		cont = 0;
		cont1 = 0;
		max = 0.40 * N * N;
		while (cont < max) {
			cont1++;
			tablero->matriz = aux;
			for (x = 0; x < N; x++) {
				for (y = 0; y < N; y++) {
					w = rand() % 100 + 0;
					if (w < 5 && aux[x][y].lock == 0) {
						z = rand() % N + 1;
						if (verificador(*tablero, z, x, y, N, visibility)
								== 0) {
							aux[x][y].valor = z;
							aux[x][y].lock = 1;
							cont++;
						}
					}
				}
			}
		}
		break;
	case 2:
		cont = 0;
		cont1 = 0;
		max = 0.30 * N * N;
		while (cont < max) {
			cont1++;
			tablero->matriz = aux;
			for (x = 0; x < N; x++) {
				for (y = 0; y < N; y++) {
					w = rand() % 100 + 0;
					if (w < 10 && aux[x][y].lock == 0) {
						z = rand() % N + 1;
						if (verificador(*tablero, z, x, y, N, visibility)
								== 0) {
							aux[x][y].valor = z;
							aux[x][y].lock = 1;
							cont++;
						}
					}
				}
			}
		}
		break;
	case 3:
		cont = 0;
		cont1 = 0;
		max = N * N;
		while (cont < max) {
			cont1++;
			tablero->matriz = aux;
			for (x = 0; x < N; x++) {
				for (y = 0; y < N; y++) {
					w = rand() % 100 + 0;
					if (aux[x][y].lock == 0) {
						z = rand() % N + 1;
						if (verificador(*tablero, z, x, y, N, visibility)
								== 0) {
							aux[x][y].valor = z;
							aux[x][y].lock = 1;
							cont++;

						}
					}
				}
			}
		}
		break;
	case 4:
		cont = 0;
		cont1 = 0;
		max = 0.15 * N * N;
		while (cont < max) {
			cont1++;
			tablero->matriz = aux;
			for (x = 0; x < N; x++) {
				for (y = 0; y < N; y++) {
					w = rand() % 100 + 0;
					if (w < 10 && aux[x][y].lock == 0) {
						z = rand() % N + 1;
						if (verificador(*tablero, z, x, y, N, visibility)
								== 0) {
							aux[x][y].valor = z;
							aux[x][y].lock = 1;
							cont++;
						}
					}
				}
			}
		}
		break;
	}
	//printf("iteraciones: %d \n\n", cont1);
	tablero->matriz = aux;
	tablero->largo = N;
	tablero->ancho = M;
	return tablero;
}

void print(board *b) {
	int x, y;
	for (x = 0; x < b->ancho; x++) {
		printf("X ->  %d|", x + 1);
		for (y = 0; y < b->ancho; y++) {

			if (b->matriz[x][y].valor != 0)
				printf(" %d |", b->matriz[x][y].valor);
			else
				printf("   |");
		}
		printf("\n");
	}
	printf("         -");
	for (x = 1; x < (b->ancho); x++) {
		printf("   -");
	}
	printf("\n");
	printf("         1");
	for (x = 1; x < (b->ancho); x++) {
		printf("   %d", x + 1);
	}
	printf("\n");
	printf("         ^   ^   ^\n");
	printf("         |   |   |\n");
	printf("         Y   Y   Y\n");
	printf("\n");
}

void play(board *b, int x, int y, int value) {
	int i, j;
	board* temp = malloc(sizeof(temp));
	casilla **aux;
	aux = malloc(b->ancho * sizeof(casilla*));
	for (i = 0; i < b->ancho; i++) {
		aux[i] = malloc(b->ancho * sizeof(casilla));
	}
	for (i = 0; i < b->ancho; i++) {
		for (j = 0; j < b->ancho; j++) {
			aux[i][j].valor = b->matriz[i][j].valor;
			aux[i][j].lock = b->matriz[i][j].lock;
		}
	}
	temp->matriz = aux;
	temp->ancho = b->ancho;
	temp->largo = b->largo;
	if (aux[x][y].valor == 0) {
		aux[x][y].valor = value;
		if (rules(temp) == 0) {
			b->matriz = aux;
			return;
		}
	}
	if (aux[x][y].valor != 0 && aux[x][y].lock == 0) {
		aux[x][y].valor = value;
		if (rules(temp) == 0) {
			b->matriz = aux;
			return;
		}
	}
	if (aux[x][y].valor != 0 && aux[x][y].lock == 1) {
		printf("No es posible modificar el valor de la celda\n\n");
		return;
	}
	return;
}

void saveGame(game *g, int *id) {

}

int lolazo(board *b) {
	int x, y, z;
	int verif = 0;
	casilla **aux;
	board* tablero = malloc(sizeof(tablero));
	aux = malloc(b->ancho * sizeof(casilla*));
	for (x = 0; x < b->ancho; x++) {
		aux[x] = malloc(b->ancho * sizeof(casilla));
	}
	for (x = 0; x < b->ancho; x++) {
		for (y = 0; y < b->ancho; y++) {
			aux[x][y].valor = 0;
			aux[x][y].lock = 0;
		}
	}
	tablero->matriz = aux;
	for (x = 0; x < b->ancho; x++) {
		for (y = 0; y < b->ancho; y++) {
			while (verif == 0) {
				if (tablero->matriz[x][y].valor == 0) {
					z = rand() % b->ancho + 1;
					tablero->matriz[x][y].valor = z;
					if (verificador(*tablero, z, x, y, b->ancho, 0) == 0)
						verif = 1;
				}
			}
			verif = 0;
			tablero->matriz[x][y].valor = 0;
		}
	}
	print(tablero);
	return 0;
}

int opcion(board *b, int x, int y) {
	int cont = 0;
	int z, aux;
	for (z = 1; z <= b->ancho; z++) {
		b->matriz[x][y].valor = z;
		if (rules(b) == 0) {
			printf("El valor es colocado\n");
			aux = z;
			cont++;
		} else
			printf("El valor NO colocado\n");
	}
	if (cont != 1)
		b->matriz[x][y].valor = 0;
	else
		b->matriz[x][y].valor = aux;
	return cont;
}

int checkBoard(board *b) {
	int valorRet, x, y;
	for (x = 0; x < b->ancho; x++) {
		for (y = 0; y < b->ancho; y++) {
			if (b->matriz[x][y].valor == 0) {
				valorRet = opcion(b, x, y);
				if (valorRet == 1) {
					x = 0;
					y = 0;
				} else if (valorRet == 0) {
					printf("Tablero sin solución\n");
					return 0;
				}
			}
		}
	}
	for (x = 0; x < b->ancho; x++) {
		for (y = 0; y < b->ancho; y++) {
			if (b->matriz[x][y].valor == 0) {
				printf("Tablero tiene más de UNA solución\n");
				return 1;
			}
		}
	}
	printf("Tablero Válido con UNA SOLA solución\n");
	for (x = 0; x < b->ancho; x++) {
		for (y = 0; y < b->ancho; y++) {
			if (b->matriz[x][y].lock == 0) {
				b->matriz[x][y].valor = 0;
			}
		}
	}
	return 1;
}

int main(void) {
	board *lol, *lol2;
	int N = 9;
	int cond = 0;
	int num = -1;
	int x, y, verif;
	int *id = (int*) malloc(sizeof(int));
	lol2 = createBoard(N, N, 2);
	verif = checkBoard(lol2);
	while (cond == 0 && verif == 1) {
		print(lol2);
		while (num < 0 || num > N) {
			printf(
					"Ingrese el valor y la coordenada x e y separados por un espacio (ej: '2 3 3'): ");
			scanf("%d %d %d", &num, &x, &y);
			printf("\n");
		}
		play(lol2, x - 1, y - 1, num);
		num = -1;
	}
	saveBoard(lol2, id);
	if(verif == 0)
		printf("Tablero No válido almacenado para analisis ");
	printf("ID: %d\n", *id);
	lol = loadBoard(*id);
	rules(lol);
	print(lol);
	return 0;
}
