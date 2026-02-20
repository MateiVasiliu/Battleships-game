
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STAT_LEN 14 // lungimea sirului prin care este data statistica
#define ALPHABET_LEN 37 // lungimea alfabetului considerat
#define BLOCK_SIZE 10
#define EPS_ERR 0.000001 // 1e-6

// DE FACUT README UL FARA DIACRITICE

typedef struct {
	char type;
	char orientation;
	int x_start;
	int y_start;
	int x_end;
	int y_end;
	int size;
	int hits;
} ship_t;

typedef struct {
	int nr_ships;
	int total_hits;
	int successful_hits;
	int total_ship_sq;
} player_stats_t;

typedef struct {
	player_stats_t *player_no;
} game_stats_t;

// afiseaza matricele corespunzatoare tablelor de joc pentru cei doi jucatori
void print_matrix(int n, int m, int **board)
{
	for (int i = 1; i <= n; i++) {
		for (int j = 1; j <= m; j++) {
			printf("%d ", board[i][j]);
		}
		printf("\n");
	}
}

// aloca vectorii ce contin date despre navele fiecarui jucator per joc
ship_t *allocate_ship_array(int n)
{
	ship_t *v = (ship_t *)calloc(n + 1, sizeof(ship_t));
		if (!v) {
			printf("malloc() a esuat\n");
			return NULL;
		}
	return v;
}

player_stats_t *allocate_player_stats_array(int n)
{
	player_stats_t *v = (player_stats_t *)calloc(n + 1,
												 sizeof(player_stats_t));
		if (!v) {
			printf("malloc() a esuat\n");
			return NULL;
		}
	return v;
}

// aloca, impreuna cu functia anterioara, vectorii ce contin date despre
// atacurile jucatorilor per joc in vederea rezolvarii statisticilor
game_stats_t *allocate_game_stats_array(int n)
{
	game_stats_t *v = (game_stats_t *)calloc(n + 1, sizeof(game_stats_t));
		if (!v) {
			printf("malloc() a esuat\n");
			return NULL;
		}
	return v;
}

double *allocate_double_array(int n)
{
	double *v = (double *)calloc(n + 1, sizeof(double));
		if (!v) {
			printf("calloc() a esuat\n");
			return NULL;
		}
	return v;
}

int *allocate_int_array(int n)
{
	int *v = (int *)calloc(n + 1, sizeof(int));
		if (!v) {
			printf("calloc() a esuat\n");
			return NULL;
		}
	return v;
}

// aloca matricele corespunzatoare tablelor de joc pentru jucatori, folosind
// indexare de la 1
int **allocate_game_matrix(int n, int m)
{
	int **a = (int **)malloc((n + 1) * sizeof(int *));
		if (!a) {
			printf("malloc() a esuat\n");
			return NULL;
		}
		for (int i = 1; i <= n; i++) {
			a[i] = (int *)calloc((m + 1), sizeof(int));
			if (!a[i]) {
				printf("calloc() a esuat\n");
				for (int j = 1; j < i; j++) {
					free(a[j]);
				}
				free(a);
				return NULL;
			}
		}
	return a;
}

// aloca matricele folosite in calculele matematice din algoritmul Hill Cipher
// folosind indexare de la 0
int **allocate_math_matrix(int n, int m)
{
	int **a = (int **)malloc(n * sizeof(int *));
		if (!a) {
			printf("malloc() a esuat\n");
			return NULL;
		}
		for (int i = 0; i < n; i++) {
			a[i] = (int *)calloc(m, sizeof(int));
			if (!a[i]) {
				printf("calloc() a esuat\n");
				for (int j = 0; j < i; j++) {
					free(a[j]);
				}
				free(a);
				return NULL;
			}
		}
	return a;
}

char *allocate_string(int n)
{
	char *string = (char *)calloc(n + 1, sizeof(char));
	if (!string) {
		printf("calloc() a esuat\n");
		return NULL;
		}
	return string;
}

// afiseaza mesajele corespunzatoare plasarii necorespunzatoare, mesaj
// personalizat pentru fiecare tip de nava si orientarea ei
void print_invalid_placing_messages(char type, char orientation,
									int x_coordinate, int y_coordinate)
{
	if (type == 'A') {
		if (orientation == 'H') {
			printf("Nava Albacore nu poate fi amplasata orizontal la "
				   "coordonatele (%d, %d).\n", x_coordinate, y_coordinate);
		} else if (orientation == 'V') {
			printf("Nava Albacore nu poate fi amplasata vertical la "
				   "coordonatele (%d, %d).\n", x_coordinate, y_coordinate);
		}
	} else if (type == 'L') {
		if (orientation == 'H') {
			printf("Nava Laffey nu poate fi amplasata orizontal la "
				   "coordonatele (%d, %d).\n", x_coordinate, y_coordinate);
		} else if (orientation == 'V') {
			printf("Nava Laffey nu poate fi amplasata vertical la "
				   "coordonatele (%d, %d).\n", x_coordinate, y_coordinate);
		}
	} else if (type == 'B') {
		if (orientation == 'H') {
			printf("Nava Belfast nu poate fi amplasata orizontal la "
				   "coordonatele (%d, %d).\n", x_coordinate, y_coordinate);
		} else if (orientation == 'V') {
			printf("Nava Belfast nu poate fi amplasata vertical la "
				   "coordonatele (%d, %d).\n", x_coordinate, y_coordinate);
		}
	} else if (type == 'Y') {
		if (orientation == 'H') {
			printf("Nava Yamato nu poate fi amplasata orizontal la "
				   "coordonatele (%d, %d).\n", x_coordinate, y_coordinate);
		} else if (orientation == 'V') {
			printf("Nava Yamato nu poate fi amplasata vertical la "
				   "coordonatele (%d, %d).\n", x_coordinate, y_coordinate);
		}
	} else if (type == 'S') {
		if (orientation == 'H') {
			printf("Nava Shinano nu poate fi amplasata orizontal la "
				   "coordonatele (%d, %d).\n", x_coordinate, y_coordinate);
		} else if (orientation == 'V') {
			printf("Nava Shinano nu poate fi amplasata vertical la "
				   "coordonatele (%d, %d).\n", x_coordinate, y_coordinate);
		}
	}
}

// determina lungimea navei pe baza tipului pentru a umple matricea folosind
// valori numerice, nu caractere, pe pozitiile corespunzatoare navei
void ship_length(int type, int *nr)
{
	if (type == 'A') {
		*nr = 1;
	} else if (type == 'L') {
		*nr = 2;
	} else if (type == 'B') {
		*nr = 3;
	} else if (type == 'Y') {
		*nr = 4;
	} else if (type == 'S') {
		*nr = 5;
	}
}

// determina coordonatele de final ale navei (dupa orientare) prin determinarea
// intinderii navei
void ship_span(char orientation, int x_coordinate, int y_coordinate, int nr,
			   int *line, int *col)
{
	if (orientation == 'H') {
		*line = x_coordinate;
		*col = y_coordinate + nr - 1;
	} else {
		*line = x_coordinate - nr + 1;
		*col = y_coordinate;
	}
}

// construieste tabla de joc punand pe pozitiile cuprinse intre coordonatele
// capetelor navei valoarea numerica corespunzatoare
void build_board(int **board, char orientation, int x_coordinate,
				 int y_coordinate, int line, int col, int nr)
{
	if (orientation == 'V') {
		for (int i = x_coordinate; i >= line; i--) {
			board[i][y_coordinate] = nr;
		}
	} else if (orientation == 'H') {
		for (int j = y_coordinate; j <= col; j++) {
			board[x_coordinate][j] = nr;
		}
	}
}

// verifica daca plasarea unei nave ar presupune suprapunerea cu o alta nava
// deja existenta pe tabla
int check_overlapping(int **board, char orientation, int x_coordinate,
					  int y_coordinate, int line, int col)
{
	if (orientation == 'V') {
		for (int i = x_coordinate; i >= line; i--) {
			if (board[i][y_coordinate] != 0) {
				return 0;
			}
		}
	} else if (orientation == 'H') {
		for (int j = y_coordinate; j <= col; j++) {
			if (board[x_coordinate][j] != 0) {
				return 0;
			}
		}
	}
	return 1;
}

// verifica daca plasarea navei ar presupune iesirea din limitele tablei sau
// daca coordonatele initiale nu se regasesc pe tabla (sunt mai mari decat
// dimensiunile citite)
int check_out_of_bounds(int line, int col, int n, int m, int x_coordinate,
						int y_coordinate)
{
	if (line < 1 || line > n || x_coordinate < 1 || x_coordinate > n ||
		y_coordinate < 1 || y_coordinate > m || col < 1 || col > m) {
		return 0;
	}
	return 1;
}

// verifica, folosind functiile anterioare, daca plasarea unei nave respecta
// toate conditiile
int check_valid_placement(int **board_p1, int **board_p2, int n, int m,
						  char type, char orientation, int x_coordinate,
						  int y_coordinate, int player)
{
	int nr;
	ship_length(type, &nr);
	int overlapping, col, line;
	ship_span(orientation, x_coordinate, y_coordinate, nr, &line, &col);

	int oob = check_out_of_bounds(line, col, n, m, x_coordinate, y_coordinate);
	if (oob == 0) {
		print_invalid_placing_messages(type, orientation, x_coordinate,
									   y_coordinate);
		return 0;
	}

	if (player == 1) {
		overlapping = check_overlapping(board_p1, orientation,
										x_coordinate, y_coordinate, line, col);
	} else {
		overlapping = check_overlapping(board_p2, orientation,
										x_coordinate, y_coordinate, line, col);
	}
	if (overlapping == 0) {
		print_invalid_placing_messages(type, orientation, x_coordinate,
									   y_coordinate);
		return 0;
	}

	return 1;
}

// simuleaza distrugerea navelor prin schimbarea valorilor din matricea
// corespunzatoare tablelor jucatorilor (0 = distrus, -1 = deja atacat)
void destroy_ship(ship_t *fleet, int **board, int i)
{
	if (board[fleet[i].x_start][fleet[i].y_start] != -1)
		board[fleet[i].x_start][fleet[i].y_start] =  -1;

	if (fleet[i].orientation == 'H') {
		for (int j = fleet[i].y_start + 1; j <= fleet[i].y_end; j++) {
			/*daca o celula a fost deja atacata nu pun valoarea 0, pentru a se
			afisa mesajul corespunzator*/
			if (board[fleet[i].x_start][j] != -1) {
				board[fleet[i].x_start][j] = 0;
			}
		}
	} else {
		for (int j = fleet[i].x_start - 1; j >= fleet[i].x_end; j--) {
			if (board[j][fleet[i].y_start] != -1) {
				board[j][fleet[i].y_start] = 0;
			}
		}
	}
}

void print_destroying_message(ship_t *fleet, int player, int i)
{
	if (fleet[i].type == 'A') {
		printf("Jucatorul %d a distrus o nava Albacore plasata intre "
			   "coordonatele (%d, %d) si (%d, %d).\n", player, fleet[i].x_start,
			   fleet[i].y_start, fleet[i].x_end, fleet[i].y_end);
	} else if (fleet[i].type == 'L') {
		printf("Jucatorul %d a distrus o nava Laffey plasata intre "
			   "coordonatele (%d, %d) si (%d, %d).\n", player, fleet[i].x_start,
			   fleet[i].y_start, fleet[i].x_end, fleet[i].y_end);
	} else if (fleet[i].type == 'B') {
		printf("Jucatorul %d a distrus o nava Belfast plasata intre "
			   "coordonatele (%d, %d) si (%d, %d).\n", player, fleet[i].x_start,
			   fleet[i].y_start, fleet[i].x_end, fleet[i].y_end);
	} else if (fleet[i].type == 'Y') {
		printf("Jucatorul %d a distrus o nava Yamato plasata intre "
			   "coordonatele (%d, %d) si (%d, %d).\n", player, fleet[i].x_start,
			   fleet[i].y_start, fleet[i].x_end, fleet[i].y_end);
	} else {
		printf("Jucatorul %d a distrus o nava Shinano plasata intre "
			   "coordonatele (%d, %d) si (%d, %d).\n", player, fleet[i].x_start,
			   fleet[i].y_start, fleet[i].x_end, fleet[i].y_end);
	}
}

void print_hitting_message(ship_t *fleet, int player, int x_attack,
						   int y_attack, int i)
{
	if (fleet[i].type == 'A') {
		printf("Jucatorul %d a lovit o nava Albacore la coordonatele (%d, %d)"
			   ".\n", player, x_attack, y_attack);
	} else if (fleet[i].type == 'L') {
		printf("Jucatorul %d a lovit o nava Laffey la coordonatele (%d, %d)"
			   ".\n", player, x_attack, y_attack);
	} else if (fleet[i].type == 'B') {
		printf("Jucatorul %d a lovit o nava Belfast la coordonatele (%d, %d)"
			   ".\n", player, x_attack, y_attack);
	} else if (fleet[i].type == 'Y') {
		printf("Jucatorul %d a lovit o nava Yamato la coordonatele (%d, %d)"
			   ".\n", player, x_attack, y_attack);
	} else {
		printf("Jucatorul %d a lovit o nava Shinano la coordonatele (%d, %d)"
			   ".\n", player, x_attack, y_attack);
	}
}

// simuleaza executarea atacurilor prin verificarea coordonatelor, adica daca
// acestea au fost deja atacate, daca s-a ratat lovitura sau daca acolo
// exista o nava/o parte din nava si distrugand dupa caz navele lovite
int execute_attack(ship_t *fleet, int **board, int x_attack, int y_attack,
				   int k, int player, player_stats_t *player_no)
{
	if (board[x_attack][y_attack] == -1) {
		printf("Coordonatele (%d, %d) au fost deja atacate de "
			   "jucatorul %d.\n", x_attack, y_attack, player);
		return 0;
	}

	if (board[x_attack][y_attack] == 0) {
		printf("Jucatorul %d a ratat o lovitura la coordonatele (%d, %d)."
			   "\n", player, x_attack, y_attack);
		board[x_attack][y_attack] = -1; //pentru a considera deja atacat
		return 0;
	}

	for (int i = 0; i < k; i++) {
		if ((x_attack == fleet[i].x_start && y_attack == fleet[i].y_start)) {
			player_no[player].successful_hits++;
			fleet[i].size = fleet[i].hits;
			destroy_ship(fleet, board, i);
			print_destroying_message(fleet, player, i);
			return 1;
		}
		if (fleet[i].orientation == 'H') {
			if (x_attack == fleet[i].x_start && (fleet[i].y_start < y_attack &&
												 y_attack <= fleet[i].y_end)) {
				if (board[x_attack][y_attack] != -1) {
					fleet[i].hits++;
					player_no[player].successful_hits++;
					board[x_attack][y_attack] = -1;
					if (fleet[i].hits == fleet[i].size) {
						destroy_ship(fleet, board, i);
						print_destroying_message(fleet, player, i);
						return 1;
					}
					print_hitting_message(fleet, player, x_attack,
										  y_attack, i);
					return 0;
				}
				return 0;
			}
		} else {
			if (y_attack == fleet[i].y_start && (fleet[i].x_start > x_attack &&
												 x_attack >= fleet[i].x_end)) {
				if (board[x_attack][y_attack] != -1) {
					fleet[i].hits++;
					player_no[player].successful_hits++;
					board[x_attack][y_attack] = -1;
					if (fleet[i].hits == fleet[i].size) {
						destroy_ship(fleet, board, i);
						print_destroying_message(fleet, player, i);
						return 1;
					}
					print_hitting_message(fleet, player, x_attack,
										  y_attack, i);
					return 0;
				}
				return 0;
			}
		}
	}

	if (board[x_attack][y_attack] == 0) {
		printf("Jucatorul %d a ratat o lovitura la coordonatele (%d, %d)."
			   "\n", player, x_attack, y_attack);
		board[x_attack][y_attack] = -1; //pentru a considera deja atacat
		return 0;
		}

	return 0;
}

// citeste coordonatele atacurilor si contorizeaza numarul total de lovituri si
// respectiv cele cu succes ale fiecarui jucator in vederea realizarii
// statisticilor si afiseaza mesajul de castig, corespunzator incheierii
// jocului
void attacks(ship_t *fleet_p1, ship_t *fleet_p2, int **board_p1, int **board_p2,
			 player_stats_t *player_no)
{
	int x_attack, y_attack, player = 1, destroyed_ships_1 = 0,
		destroyed_ships_2 = 0, k1 = player_no[1].nr_ships,
		k2 = player_no[2].nr_ships;

	while (destroyed_ships_1 < k2 && destroyed_ships_2 < k1) {
		scanf("%d%d", &x_attack, &y_attack);
		if (player == 1) {
			destroyed_ships_2 += execute_attack(fleet_p2, board_p2, x_attack,
												y_attack, k2, player,
												player_no);
			player_no[player].total_hits++;
		} else {
			destroyed_ships_1 += execute_attack(fleet_p1, board_p1, x_attack,
											   y_attack, k1, player, player_no);
			player_no[player].total_hits++;
		}
		if (player == 1) {
			player = 2;
		} else {
			player = 1;
		}
	}

	if (destroyed_ships_1 == k1) {
		printf("Jucatorul 2 a castigat.\n");
	} else {
		printf("Jucatorul 1 a castigat.\n");
	}
}

// converteste numarul dat in statistica prin caractere-cifra in numar intreg
int determine_number_from_string(char *initial_text, int places)
{
	int nr = 0;
	for (int i = 0; i < places; i++) {
		nr = nr * 10 + (initial_text[i] - '0');
	}
	return nr;
}

// converteste numarul de meciuri necesare in functie de optiune din numar
// intreg in sir de caractere pentru a il concatena ulterior la string-ul de
// afisat
void make_string_from_number(char *aux, int nr)
{
	int i = 0;
	if (nr == 0) {
		aux[0] = '0';
		aux[1] = '\0';
	} else {
		while (nr > 0) {
			aux[i++] = (nr % 10) + '0';
			nr /= 10;
		}
		aux[i] = '\0';
	}

	int len = strlen(aux);
	for (int i = 0; i < len / 2; i++) {
		char tmp = aux[i];
		aux[i] = aux[len - i - 1];
		aux[len - i - 1] = tmp;
	}
}

// truncheaza valorile calculate ale acuratetilor la doua zecimale pentru a
// fi pusa valoarea in sirul final
double truncate(double nr, int decimals)
{
	double pow_of_10 = 1.0;
	for (int i = 0; i < decimals; i++) {
		pow_of_10 *= 10.0;
	}
	int aux = (int)(nr * pow_of_10);
	return (double)aux / pow_of_10;
}

// creeaza sirul final de afisat/criptat concatenand acuratetea calculata cu
// doua zecimale si numarul de meciuri determinat in formatul cerut: aaa.aa.n
void create_final_text(char *final_text, double accuracy, int nr)
{
	accuracy = truncate(accuracy, 2);
	int acc_int = (int)accuracy, i = 0;

	if (acc_int == 100) {
		final_text[i++] = '1';
		final_text[i++] = '0';
		final_text[i++] = '0';
		final_text[i++] = '.';
		final_text[i++] = '0';
		final_text[i++] = '0';
	} else {
		/* afisarea unui numar de doua cifre in format de 3 cifre in partea
		intreaga si doua cifre in partea zecimala, separate prin punct */
		final_text[i++] = '0';
		final_text[i++] = (acc_int / 10 % 10) + '0';
		final_text[i++] = (acc_int % 10) + '0';
		final_text[i++] = '.';
		int acc_frac = (int)(accuracy * 100 + EPS_ERR) % 100;
		// ultimele doua cifre = partea fractionara
		final_text[i++] = (acc_frac / 10 % 10) + '0';
		final_text[i++] = (acc_frac % 10) + '0';
	}
	final_text[i++] = '.';

	int copy_nr = nr;
	if (copy_nr < 10) {
		final_text[i++] = (nr % 10) + '0';
	} else if (copy_nr < 99) {
		final_text[i++] = (nr / 10 % 10) + '0';
		final_text[i++] = (nr % 10) + '0';
	} else {
		int digits = 0;
		while (copy_nr > 0) {
			digits++;
			copy_nr /= 10;
		}
		char *aux = allocate_string(digits);
		make_string_from_number(aux, nr);
		for (int k = 0 ; k < digits; k++) {
			final_text[i++] = aux[k];
		}
		free(aux);
	}
	final_text[i] = '\0';
}

// face calculele necesare optiunii 1 de statistica, calculand media ponderata
// si determinand care ar fi castigul adus de modificarea acuratetii unui joc
// din cea actuala in 100%, sortand apoi dupa jocurile cu impact cat mai mare
void option_1(game_stats_t *game_stats, int player, double accuracy, int j,
			  char *final_text)
{
	double numerator = 0.0, denominator = 0.0;
	// numaratorul si numitorul din formula mediei ponderate
	double *match_acc = allocate_double_array(j);
	double *ship_count = allocate_double_array(j);
	double *gain = allocate_double_array(j);
	int *sorted_indexes = allocate_int_array(j);

	for (int i = 1; i <= j; i++) {
		sorted_indexes[i] = i;
		ship_count[i] = (double)game_stats[i].player_no[player].nr_ships;
		denominator += ship_count[i];

		int cp_successful_hits =
			game_stats[i].player_no[player].successful_hits;
		int cp_total_hits = game_stats[i].player_no[player].total_hits;

		if (cp_total_hits > 0)
			match_acc[i] = (double)cp_successful_hits / (double)cp_total_hits;

		numerator += match_acc[i] * ship_count[i];
		// cat se castiga daca facem acuratetea meciului 100%
		gain[i] = (1.0 - match_acc[i]) * ship_count[i];
	}
	if (numerator == 0.0) {
		create_final_text(final_text, 0.0, 0);
		free(match_acc);
		free(ship_count);
		free(gain);
		free(sorted_indexes);
		return;
	}

	double current_acc = numerator / denominator;
	double target_acc = accuracy / 100.0;
	int cnt = 0;
	if (current_acc + EPS_ERR >= target_acc) {
		create_final_text(final_text, current_acc * 100.0, cnt);
		free(match_acc);
		free(ship_count);
		free(gain);
		free(sorted_indexes);
		return;
	}
	// sortare după gain descrescător - sortez vectorul in care retin indicii
	// nu meciurile efective, astfel incat pe prima pozitie se gaseste indexul
	// jocului cu cel mai mare castig
	for (int i = 1; i <= j; i++) {
		for (int k = i + 1; k <= j; k++) {
			if (gain[sorted_indexes[k]] > gain[sorted_indexes[i]]) {
				int tmp = sorted_indexes[i];
				sorted_indexes[i] = sorted_indexes[k];
				sorted_indexes[k] = tmp;
			}
		}
	}

	double new_sum = numerator;
	// verific modificarea acuratetilor prin modificarea acuratetii jocurilor
	// care au cel mai mare impact si as avea cel mai mare castig
	for (int p = 1; p <= j; p++) {
		int id = sorted_indexes[p];
		new_sum += gain[id];
		cnt++;
		double new_avg = new_sum / denominator;
		// se opreste atunci cand am gasit o acuratete care satisface, adica
		// este mai mare sau egala cu cea citita
		if (new_avg + EPS_ERR >= target_acc)
			break;
	}
	create_final_text(final_text, current_acc * 100.0, cnt);

	free(match_acc);
	free(ship_count);
	free(gain);
	free(sorted_indexes);
}

// face calculele necesare optiunii 2 de statistica, determinand acuratetea pe
// meci si cate lovituri in plus ar fi trebuit sa fie cu succes pentru a avea
// acuratetea data
void option_2(game_stats_t *game_stats, int game_id, int player, double
			  accuracy, char *final_text)
{
	int cp_successful_hits =
		game_stats[game_id].player_no[player].successful_hits,
		cp_total_hits = game_stats[game_id].player_no[player].total_hits,
		added_hits = 0;
	double match_accuracy = (double)cp_successful_hits / cp_total_hits *
							100.0;
	double cp_match_acc = match_accuracy;

	while (cp_match_acc < accuracy) {
		added_hits++;
		cp_successful_hits++;
		cp_match_acc = (double)cp_successful_hits / cp_total_hits * 100.0;
	}

	create_final_text(final_text, match_accuracy, added_hits);
}

// face calculele necesare optiunii 3 de statistica, calculand media ponderata
// si determinand care ar fi pierderea adusa de modificarea acuratetii unui joc
// din cea actuala in 0%, sortand apoi dupa jocurile cu impact cat mai mic
void option_3(game_stats_t *game_stats, int player, double accuracy, int j,
			  char *final_text)
{
	double numerator = 0.0, denominator = 0.0;
	// numitorul si numaratorul din calculul mediei ponderate
	double *match_acc = allocate_double_array(j);
	double *ship_count = allocate_double_array(j);
	double *loss = allocate_double_array(j);
	int *sorted_indexes = allocate_int_array(j);

	for (int i = 1; i <= j; i++) {
		sorted_indexes[i] = i;

		ship_count[i] = (double)game_stats[i].player_no[player].nr_ships;
		denominator += ship_count[i];

		int cp_successful_hits =
			game_stats[i].player_no[player].successful_hits;
		int cp_total_hits = game_stats[i].player_no[player].total_hits;

		if (cp_total_hits > 0)
			match_acc[i] = (double)cp_successful_hits / (double)cp_total_hits;

		numerator += match_acc[i] * ship_count[i];
		// cat se pierde daca facem acuratetea meciului 0%
		loss[i] = match_acc[i] * ship_count[i];
	}
	if (numerator == 0.0) {
		create_final_text(final_text, 0.0, 0);
		free(match_acc);
		free(ship_count);
		free(loss);
		free(sorted_indexes);
		return;
	}
	double current_acc = numerator / denominator;
	double target_acc = accuracy / 100.0;
	int cnt = 0;

	if (current_acc + EPS_ERR < target_acc) {
		create_final_text(final_text, current_acc * 100.0, cnt);
		free(match_acc);
		free(ship_count);
		free(loss);
		free(sorted_indexes);
		return;
	}
	// sortare dupa loss crescător
	for (int i = 1; i <= j; i++) {
		for (int k = i + 1; k <= j; k++) {
			if (loss[sorted_indexes[k]] < loss[sorted_indexes[i]]) {
				int tmp = sorted_indexes[i];
				sorted_indexes[i] = sorted_indexes[k];
				sorted_indexes[k] = tmp;
			}
		}
	}
	// verific modificarea acuratetilor prin modificarea acuratetii jocurilor
	// care au cel mai mic impact si as avea cea mai mica pierdere
	double aux = numerator;
	for (int p = 1; p <= j; p++) {
		int id = sorted_indexes[p];
		double candidate = aux - loss[id];
		double new_avg = candidate / denominator;
		if (new_avg + EPS_ERR >= target_acc) {
			aux = candidate;
			cnt++;
		}
	}
	create_final_text(final_text, current_acc * 100.0, cnt);

	free(match_acc);
	free(ship_count);
	free(loss);
	free(sorted_indexes);
}

// face calculele necesare optiunii 4 de statistica, determinand acuratetea pe
// meci si cate lovituri ar fi putut rata jucatorul pentru a avea acuratetea
// data
void option_4(game_stats_t *game_stats, int game_id, int player, double
			  accuracy, char *final_text)
{
	int cp_successful_hits =
		game_stats[game_id].player_no[player].successful_hits,
		cp_total_hits = game_stats[game_id].player_no[player].total_hits,
		removed_hits = 0;

	double match_accuracy = (double)cp_successful_hits / cp_total_hits * 100.0;
	// scad 1 pt a verifica acuratetea urmatoare (ca sa nu am o eliminare
	// in plus decat ar trebui)
	double next_acc = (double)(cp_successful_hits - 1) / cp_total_hits * 100.0;

	while (next_acc + EPS_ERR >= accuracy) {
		removed_hits++;
		cp_successful_hits--;
		next_acc = (double)(cp_successful_hits - 1) / cp_total_hits * 100.0;
	}

	create_final_text(final_text, match_accuracy, removed_hits);
}

void statistics(char *initial_text, char *final_text, game_stats_t *game_stats,
				int j)
{
	int player;
	if (initial_text[0] == 'O') {
		player = 1;
	} else if (initial_text[0] == 'T') {
		player = 2;
	}
	char no_of_games = initial_text[1];
	int game_id = determine_number_from_string(initial_text + 2, 3);
	//indicele jocului se afla de la caracterul 2 la caracterul 4
	int acc_int = determine_number_from_string(initial_text + 5, 3);
	// partea intreaga a acuratetii se afla de la caracterul 5 la caracterul 7
	int acc_dec = determine_number_from_string(initial_text + 9, 2);
	// partea fractionara a acuratetii se afla de la caracterul 8 la
	//caracterul 10, dar ignor punctul pentru ca fac variabila double
	// / 100 pentru ca se cer 2 zecimale
	double accuracy = acc_int + acc_dec / 100.;
	char *type = initial_text + 11;

	if (no_of_games == 'U') {
		if (strcmp(type, "MIN") == 0) {
			option_2(game_stats, game_id, player, accuracy, final_text);
		} else if (strcmp(type, "MAX") == 0) {
			option_4(game_stats, game_id, player, accuracy, final_text);
		}
	} else if (no_of_games == 'T') {
		if (strcmp(type, "MIN") == 0) {
			option_1(game_stats, player, accuracy, j, final_text);
		} else if (strcmp(type, "MAX") == 0) {
			option_3(game_stats, player, accuracy, j, final_text);
		}
	}
}

// calculeaza matricea obtinuta din inmultirea a alte doua matrice, una de
// dimensiune n*n si una n*1 - asa fiind input-urile din Hill Cipher
void multiply_matrix(int n, int p, int **a, int **b, int **c)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < p; j++) {
			c[i][j] = 0;
			for (int r = 0; r < n; r++) {
				c[i][j] += a[i][r] * b[r][j];
			}
		}
	}
}

// calculeza determinantul matricei folosind dezvoltarea pe prima linie; se
// calculeaza recursiv pana la o matrice 1x1 cu determinantul egal cu acel
// element
int calculate_matrix_determinant(int k, int **key)
{
	int sign, det = 0;
	if (k == 1) {
		det = key[0][0];
		return det;
	}
	for (int i = 0; i < k; i++) {
		int **mat = allocate_math_matrix(k - 1, k - 1);
		int mat_row = 0;
		for (int j = 1; j < k; j++) {
			int mat_col = 0;
			for (int q = 0; q < k; q++) {
				if (q != i) {
					mat[mat_row][mat_col] = key[j][q];
					mat_col++;
				}
			}
			mat_row++;
		}

		if (i % 2 == 0) {
			sign = 1;
		} else {
			sign = -1;
		}
		det = det + sign * key[0][i] *
			  calculate_matrix_determinant(k - 1, mat);
		for (int cnt = 0; cnt < k - 1; cnt++) {
			free(mat[cnt]);
		}
		free(mat);
	}
	return det;
}

// determina inversul multiplicativ
int calculate_multiplicative_inverse(int nr)
{
	nr = (nr % 37 + 37) % 37;
	// (nr*x)%37 = 1
	for (int x = 1; x < 37; x++) {
		if ((nr * x) % 37 == 1) {
			return x;
		}
	}
	return -1;
}

// calculeaza un minor - folosit in calculul cofactorilor ce formeaza matricea
// adjuncta
void calculate_minor(int row, int col, int k, int **key, int **minor)
{
	int r = 0;
	for (int i = 0; i < k; i++) {
		if (i != row) {
			int c = 0;
			for (int j = 0; j < k; j++) {
				if (j != col) {
					minor[r][c] = key[i][j];
					c++;
				}
			}
			r++;
		}
	}
}

// calculeaza matricea adjuncta pentru a afla ulterior inversa
void calculate_adjugate_matrix(int k, int **key, int **adj)
{
	int sign;
	int **minor = allocate_math_matrix(k - 1, k - 1);

	for (int i = 0; i < k; i++) {
		for (int j = 0; j < k; j++) {
			if ((i + j) % 2 == 0) {
				sign = 1;
			} else {
				sign = -1;
			}
			calculate_minor(i, j, k, key, minor);
			int cofactor = sign * calculate_matrix_determinant(k - 1, minor);
			adj[j][i] = cofactor;
		}
	}

	for (int q = 0; q < k - 1; q++) {
		free(minor[q]);
	}
	free(minor);
}

// aplica % 37 pe elemtele inversei
void inverse_times_adjugate_modulo_37(int k, int inverse, int **adj)
{
	for (int i = 0; i < k; i++) {
		for (int j = 0; j < k; j++) {
			adj[i][j] *= inverse;
		}
	}
	for (int i = 0; i < k; i++) {
		for (int j = 0; j < k; j++) {
			adj[i][j] = (adj[i][j] % 37 + 37) % 37;
		}
	}
}

// determina dimensiunile matricei egale cu radical din lungimea cheii citite
int my_sqrt(int n)
{
	int root = 0;
	while ((root + 1) * (root + 1) <= n) {
		root++;
	}
	return root;
}

// creeaza alfabetul corespunzator Hill Cipher
void create_alphabet(char *alphabet)
{
	for (int i = 0; i < ALPHABET_LEN; i++) {
		if (i < 10) {
			alphabet[i] = i + '0';
		} else if (i == 36) {
			alphabet[i] = '.';
		} else {
			alphabet[i] = 'A' + i - 10;
		}
	}
}

// formeaza matricea mesajului ce contine statistica din litere in codul
// asociat in alfabet
void create_message_matrix(int k, char *initial_text, int **message)
{
	for (int i = 0; i < k; i++) {
		if (initial_text[i] >= '0' && initial_text[i] <= '9') {
			message[i][0] = initial_text[i] - '0';
		} else if (initial_text[i] >= 'A' && initial_text[i] <= 'Z') {
			message[i][0] = initial_text[i] - 'A' + 10;
		} else if (initial_text[i] == '.') {
			message[i][0] = 36;
		}
	}
}

// formeaza matricea cheii citite din litere in codul asociat in alfabet
void create_key_matrix(int k, char *key_text, int **key)
{
	int pos = 0;
	for (int i = 0; i < k; i++) {
		for (int j = 0; j < k; j++) {
			if (key_text[pos] >= '0' && key_text[pos] <= '9') {
				key[i][j] = key_text[pos] - '0';
			} else if (key_text[pos] >= 'A' && key_text[pos] <= 'Z') {
				key[i][j] = key_text[pos] - 'A' + 10;
			} else if (key_text[pos] == '.') {
				key[i][j] = 36;
			}
			pos++;
		}
	}
}

// creaza textul criptat/decriptat pe baza matricei realizate
void create_text_from_matrix(int k, int **result, char *final_text,
							 char *alphabet)
{
	int i;
	for (i = 0; i < k; i++) {
		int value = result[i][0] % 37;
		if (value >= 0 && value <= 9) {
			final_text[i] = value + '0';
		} else {
			final_text[i] = alphabet[value];
		}
	}
	final_text[i] = '\0';
}

// parcurge toti pasii necesari criptarii unui bloc din mesajul dat
void encrypt_message(int k, char *initial_text, char *key_text,
					 char *final_text, char *alphabet)
{
	int **message = allocate_math_matrix(k, 1);
	int **key = allocate_math_matrix(k, k);
	int **result = allocate_math_matrix(k, 1);

	create_message_matrix(k, initial_text, message);
	create_key_matrix(k, key_text, key);

	// produsul intre matricea cheii si a mesajului citit
	multiply_matrix(k, 1, key, message, result);

	create_text_from_matrix(k, result, final_text, alphabet);

	for (int i = 0; i < k; i++) {
		free(message[i]);
		free(key[i]);
		free(result[i]);
	}
	free(message);
	free(key);
	free(result);
}

// initiaza criptarea mesajului lung, impartindu-l in blocuri de dimensiunea
// radicalului cheii citite si concateneaza criptarea realizata pe blocuri la
// sirul final
void encrypt_long_message(int k, char *initial_text, char *final_text,
						  char *key_text, char *alphabet)
{
	int len_msg = strlen(initial_text);
	int full_blocks = len_msg / k, last_block_len = len_msg % k, end = 0;
	char *ptr_block, *ptr_final;

	for (int block = 0; block < full_blocks; block++) {
		// pointer catre locul unde incepe sub-blocul
		ptr_block = initial_text + block * k;
		ptr_final = final_text + end;
		encrypt_message(k, ptr_block, key_text, ptr_final, alphabet);
		end += k; // finalul blocului
	}

	if (last_block_len > 0) {
		ptr_block = initial_text + full_blocks * k;
		ptr_final = final_text + end;
		char *small_key_text = allocate_string(last_block_len *
											   last_block_len);
		strncpy(small_key_text, key_text, last_block_len * last_block_len);
		small_key_text[last_block_len * last_block_len] = '\0';
		encrypt_message(last_block_len, ptr_block, small_key_text, ptr_final,
						alphabet);
		free(small_key_text);
		end += last_block_len;
	}
	final_text[end] = '\0';
}

// parcurge toti pasii necesari criptarii unui bloc din mesajul dat
void decrypt_message(int k, char *initial_text, char *final_text,
					 char *key_text, char *alphabet)
{
	int **message = allocate_math_matrix(k, 1);
	int **key = allocate_math_matrix(k, k);
	int **result = allocate_math_matrix(k, 1);

	create_message_matrix(k, initial_text, message);
	create_key_matrix(k, key_text, key);

	/*calculeaza determinantul, inversul, adjuncta matricei cheii, aplica % 37
	pe ea, realizeaza ultima inmultire si genereaza mesajul final decriptat*/
	int **adj = allocate_math_matrix(k, k);
	int det = calculate_matrix_determinant(k, key);
	int nr = (det % 37 + 37) % 37; // restul pozitiv
	int inverse = calculate_multiplicative_inverse(nr);

	calculate_adjugate_matrix(k, key, adj);
	inverse_times_adjugate_modulo_37(k, inverse, adj);
	multiply_matrix(k, 1, adj, message, result);

	create_text_from_matrix(k, result, final_text, alphabet);

	for (int i = 0; i < k; i++) {
		free(message[i]);
		free(key[i]);
		free(result[i]);
		free(adj[i]);
	}
	free(message);
	free(key);
	free(result);
	free(adj);
}

// initiaza decriptarea mesajului lung, impartindu-l in blocuri de dimensiunea
// radicalului cheii citite si concateneaza decriptarea realizata pe blocuri la
// sirul final
void decrypt_long_message(int k, char *initial_text, char *final_text,
						  char *key_text, char *alphabet)
{
	int len_msg = strlen(initial_text);
	int full_blocks = len_msg / k, last_block_len = len_msg % k, end = 0;
	char *ptr_block, *ptr_final;

	for (int block = 0; block < full_blocks; block++) {
		// pointer catre locul unde incepe sub-blocul
		ptr_block = initial_text + block * k;
		ptr_final = final_text + end;
		decrypt_message(k, ptr_block, ptr_final, key_text, alphabet);
		end += k; // finalul blocului
	}

	if (last_block_len > 0) {
		ptr_block = initial_text + full_blocks * k;
		ptr_final = final_text + end;
		char *small_key_text = allocate_string(last_block_len *
											   last_block_len);
		strncpy(small_key_text, key_text, last_block_len * last_block_len);
		small_key_text[last_block_len * last_block_len] = '\0';
		decrypt_message(last_block_len, ptr_block, ptr_final, small_key_text,
						alphabet);
		free(small_key_text);
		end += last_block_len;
	}
	final_text[end] = '\0';
}

// citeste cheia si sirurile de statistici si apeleaza functiile necesare:
// (prelucarea statisticii si/sau criptarea/decriptarea)
void game_extension(game_stats_t *game_stats, int j)
{
	char *alphabet = allocate_string(ALPHABET_LEN);
	create_alphabet(alphabet);
	char *key_text = allocate_string(10);

	scanf(" %s", key_text);
	if (strcmp(key_text, "Q") == 0) {
		free(key_text);
		free(alphabet);
		return; // opreste functia pentru ca s-a citit Q = finalul programului
	}
	int len = strlen(key_text);
	int key_mat_len = my_sqrt(len);

	// informatia din statistica are mereu 14 caractere
	char *initial_text = allocate_string(STAT_LEN);
	scanf(" %s", initial_text);

	while (strcmp(initial_text, "Q") != 0) {
		char *final_text = allocate_string(100);
		if (strcmp(key_text, "-") != 0) {
			decrypt_long_message(key_mat_len, initial_text, final_text,
								 key_text, alphabet);
			// textul decriptat vine din functie prin variabila final_text
			// si ulterior trebuie sa il prelucrez ca iniital_text
			strcpy(initial_text, final_text);
			statistics(initial_text, final_text, game_stats, j);
			// textul rezultat vine din functie prin variabila final_text
			// si ulterior trebuie sa il criptez din variabila iniital_text
			strcpy(initial_text, final_text);
			encrypt_long_message(key_mat_len, initial_text, final_text,
								 key_text, alphabet);
		} else {
			statistics(initial_text, final_text, game_stats, j);
		}
		printf("%s\n", final_text);
		free(final_text);
		scanf(" %s", initial_text);
	}
	free(initial_text);
	free(key_text);
	free(alphabet);
}

// colecteaza, pentru fiecare nava plasata corespunzator, detalii despre tip,
// orientare si dimesniune pentru a putea modifica dupa atacuri
void set_fleet(ship_t *fleet, int k, char type, char orientation,
			   int x_coordinate, int y_coordinate, int line, int col, int nr)
{
	fleet[k].type = type;
	fleet[k].orientation = orientation;
	fleet[k].x_start = x_coordinate;
	fleet[k].y_start = y_coordinate;
	fleet[k].x_end = line;
	fleet[k].y_end = col;
	fleet[k].size = nr;
}

// plaseaza navele pe tabla, verificand corectitudinea plasarii
void place_ships(int n, int m, int *k1, int *k2, int max_ships, int **board_p1,
				 int **board_p2, ship_t *fleet_p1, ship_t *fleet_p2,
				 int *cell_count_p1, int *cell_count_p2)
{
	char type, orientation;
	int x_coordinate, y_coordinate, player = 1, nr;
	while (*k1 < max_ships || *k2 < max_ships) {
		scanf(" %c %c %d %d", &type, &orientation, &x_coordinate,
			  &y_coordinate);
		int placed = check_valid_placement(board_p1, board_p2, n, m, type,
										   orientation, x_coordinate,
										   y_coordinate, player);
		if (placed == 1) {
			ship_length(type, &nr);
			int line, col;
			ship_span(orientation, x_coordinate, y_coordinate, nr, &line,
					  &col);
			if (player == 1) {
				build_board(board_p1, orientation,
							x_coordinate, y_coordinate, line, col, nr);
				set_fleet(fleet_p1, *k1, type, orientation, x_coordinate,
						  y_coordinate, line, col, nr);
				(*k1)++;
				(*cell_count_p1) += nr;
				player = 2;
			} else {
				build_board(board_p2, orientation,
							x_coordinate, y_coordinate, line, col, nr);
				set_fleet(fleet_p2, *k2, type, orientation, x_coordinate,
						  y_coordinate, line, col, nr);
				(*k2)++;
				(*cell_count_p2) += nr;
				player = 1;
			}
		}
	}
}

int main(void)
{
	int j, n, m;
	scanf("%d", &j);
	game_stats_t *game_stats = allocate_game_stats_array(j);
	for (int game = 1; game <= j; game++) {
		scanf("%d%d", &n, &m);

		int max_A = n * m / 20, max_L = n * m / 30, max_B = n * m / 40,
			max_Y = n * m / 55, max_S = n * m / 70, k1 = 0, k2 = 0,
			cell_count_p1 = 0, cell_count_p2 = 0;
		int max_ships = max_A + max_L + max_B + max_Y + max_S;
		game_stats[game].player_no = allocate_player_stats_array(2);
		int **board_p1 = allocate_game_matrix(n, m);
		int **board_p2 = allocate_game_matrix(n, m);
		ship_t *fleet_p1 = allocate_ship_array(max_ships);
		ship_t *fleet_p2 = allocate_ship_array(max_ships);

		place_ships(n, m, &k1, &k2, max_ships, board_p1, board_p2, fleet_p1,
					fleet_p2, &cell_count_p1, &cell_count_p2);

		/*seteaza numarul de nave si de lovituri totale pentru fiecare meci,
		pentru fiecare jucator*/
		game_stats[game].player_no[1].nr_ships = k1;
		game_stats[game].player_no[2].nr_ships = k2;
		game_stats[game].player_no[1].total_ship_sq = cell_count_p1;
		game_stats[game].player_no[2].total_ship_sq = cell_count_p2;

		print_matrix(n, m, board_p1);
		printf("\n");
		print_matrix(n, m, board_p2);

		attacks(fleet_p1, fleet_p2, board_p1, board_p2,
				game_stats[game].player_no);

		for (int i = 1; i <= n; i++) {
			free(board_p1[i]);
			free(board_p2[i]);
		}
		free(board_p1);
		free(board_p2);
		free(fleet_p1);
		free(fleet_p2);
	}
	game_extension(game_stats, j);

	for (int game = 1; game <= j; game++) {
		free(game_stats[game].player_no);
	}
	free(game_stats);

	return 0;
}
