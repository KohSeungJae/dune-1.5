#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"


void intro(void);
void outro(void);
void cursor_move(DIRECTION dir, int n);

/* ================= control =================== */
int sys_clock = 0;
POSITION cursor = { 0, 0 };
SELECTION selection = { 0 };

/* ================= game data =================== */
RESOURCE resource = {
	.spice = 100,
	.spice_max = 100,
	.population = 5,
	.population_max = 100
};
UNIT unit_map[MAP_HEIGHT][MAP_WIDTH] = { 0 };
BUILDING building_map[MAP_HEIGHT][MAP_WIDTH] = { 0 };
NATURE_INFO* nature_map[MAP_HEIGHT][MAP_WIDTH] = { 0 };
SKY eagle = {
	.pos = {4, 0},
	.dest = {5, MAP_WIDTH - 1},
	.move_period = 500,
	.color = COLOR_PURPLE,
	.repr = 'E',
	.state_message = {
		.about_size = 2,
		.size = 2,
		.message = {
			"[사막독수리]",
			"이 사막에서 가장 평화로운 존재이다."
		}
	}
};
bool map_change[MAP_HEIGHT][MAP_WIDTH] = { 0 };


UNIT_INFO
b_havester_info = {
	.repr = 'H',
	.name = "하베스터",
	.color = COLOR_BLUE,
	.cost = 5,
	.population = 5,
	.max_hp = 70,
	.move_period = 2000,
	.state_message = {
		.size = 8,
		.about_size = 2,
		.message = {
			"[하베스터] (아트레이디스)",
			"스파이스를 수확해 배달한다.",
			"생산 비용 : 5",
			"인구수 : 5",
			"이동 주기 : 2초",
			"공격력 : 0",
			"체력 : 70",
			"시야 : 0",
		}
	},
	.cmd_message = {
		.size = 2,
		.message = {
			"H: 스파이스 채집",
			"M: 이동"
		}
	}
},
r_havester_info = {
	.repr = 'H',
	.name = "하베스터",
	.color = COLOR_RED,
	.cost = 5,
	.population = 5,
	.max_hp = 70,
	.move_period = 2000,
	.state_message = {
		.size = 8,
		.about_size = 2,
		.message = {
			"[하베스터] (하코넨)",
			"스파이스를 수확해 배달한다.",
			"생산 비용 : 5",
			"인구수 : 5",
			"이동 주기 : 2초",
			"공격력 : 0",
			"체력 : 70",
			"시야 : 0",
		}
	}
},
sandworm_info = {
	.repr = 'W',
	.name = "샌드웜",
	.color = COLOR_DY,
	.damage = 9999999,
	.max_hp = 9999999,
	.move_period = 2500,
	.attack_period = 10000,
	.state_message = {
		.size = 4,
		.about_size = 4,
		.message = {
			"[샌드웜]",
			"사막의 무시무시한 포식자",
			"바위 위는 공격하지 못하는 듯 하다.",
			"가끔씩 스파이스를 배출한다."
		}
	}
};

BUILDING_INFO
b_base_info = {
	.repr = 'B',
	.name = "본진",
	.color = COLOR_BLUE,
	.cost = 0,
	.size = 2,
	.max_hp = 50,
	.state_message = {
		.size = 2,
		.about_size = 2,
		.message = {
			"[본진] (아트레이디스)",
			"하베스터를 생산 할 수 있다."
		}
	},
	.cmd_message = {
		.size = 1,
		.message = {
			"H : 하베스터 생산"
		}
	}
},
r_base_info = {
	.repr = 'B',
	.name = "본진",
	.color = COLOR_RED,
	.cost = 0,
	.size = 2,
	.max_hp = 50,
	.state_message = {
		.size = 2,
		.about_size = 2,
		.message = {
			"[본진] (하코넨)",
			"하베스터를 생산 할 수 있다."
		}
	}
},
b_plate_info = {
	.repr = 'P',
	.name = "장판",
	.color = 1,
	.cost = 0,
	.size = 2,
	.max_hp = -1,
	.state_message = {
		.size = 2,
		.about_size = 2,
		.message = {
			"[장판] (아트레이디스)",
			"위에 건물을 건설 할 수 있다."
		}
	}
},
r_plate_info = {
	.repr = 'P',
	.name = "장판",
	.color = 4,
	.cost = 0,
	.size = 2,
	.max_hp = -1,
	.state_message = {
		.size = 2,
		.about_size = 2,
		.message = {
			"[장판](하코넨)",
			"위에 건물을 건설 할 수 있다."
		}
	}
},
spice_info = {
	.repr = 'S',
	.name = "스파이스",
	.color = 207,
	.cost = 0,
	.size = 1,
	.max_hp = 9,
	.state_message = {
		.size = 3,
		.about_size = 3,
		.message = {
			"[스파이스]",
			"온통 모래뿐인 사막에서의 귀중한 자원이다.",
			"하베스터를 통해 얻을 수 있다.",
		}
	}
};

NATURE_INFO
desert_info = {
	.repr = ' ',
	.color = COLOR_YELLOW,
	.state_message = {
		.size = 3,
		.about_size = 3,
		.message = {
			"[사막 지형]",
			"물 한 방울도 찾아볼 수 없다.",
			"모래 속에는 무엇인가 살고 있는 것 같다..."
			}
	}
},
rock_info = {
	.repr = 'R',
	.color = COLOR_GARY,
	.state_message = {
		.size = 2,
		.about_size = 2,
		.message = {
			"[바위]",
			"평범한 바위이다."
		}
	}
};



/* ================= utility =================== */
POSITION Q[MAP_HEIGHT * MAP_WIDTH]; 
int dist[MAP_HEIGHT][MAP_WIDTH];	
int double_click_distance = 5;
int vist_timer = -1;
KEY prev_key;
bool sand_worm_on = 0;
bool unit_exist = 1;

/* ================= function =================== */
BUILDING new_building(BUILDING_INFO* building_info, POSITION pos) {
	return (BUILDING) {
		.exist = 1,
			.pos = pos,
			.hp = building_info->max_hp,
			.info_p = building_info
	};
}
UNIT new_unit(UNIT_INFO* unit_info, POSITION pos) {
	return (UNIT) {
		.exist = 1,
			.pos = pos,
			.dest = { -1, -1 },
			.next_move_time = unit_info->move_period,
			.next_attack_time = unit_info->attack_period,
			.hp = unit_info->max_hp,
			.info_p = unit_info
	};
}
inline void init_rock(POSITION pos) {
	nature_map[pos.x][pos.y] = &rock_info;
}
inline void init_4rock(POSITION pos) {
	for (int i = pos.x; i < pos.x + 2; i++) {
		for (int j = pos.y; j < pos.y + 2; j++) {
			POSITION pos = { i, j };
			init_rock(pos);
		}
	}
}
inline void init_total_rock() {
	POSITION pos[5] = { {5,16}, {MAP_HEIGHT - 7, MAP_WIDTH - 18},  { MAP_HEIGHT - 7,10 }, {6, MAP_WIDTH - 11}, {MAP_HEIGHT / 2, MAP_WIDTH / 2} };
	init_4rock(pos[0]);
	init_4rock(pos[1]);
	init_rock(pos[2]);
	init_rock(pos[3]);
	init_rock(pos[4]);
}
void unit_move(POSITION pos, POSITION dest) {
	if (pos.x == dest.x && pos.y == dest.y) {
		return;
	}
	unit_map[dest.x][dest.y] = unit_map[pos.x][pos.y];
	unit_map[dest.x][dest.y].pos = dest;
	unit_map[pos.x][pos.y].exist = 0;

	map_change[pos.x][pos.y] = 1;
	map_change[dest.x][dest.y] = 1;
}
void attack_unit(UNIT_INFO* attacker, UNIT* victim) {
	(*victim).hp = ((*victim).hp - attacker->damage <= 0) ? 0 : (*victim).hp - attacker->damage;

	char buff[100];
	snprintf(buff, 100, "%s가 %s에게 공격당했습니다. (남은체력 : %d)", (*victim).info_p->name, attacker->name, (*victim).hp);
	display_system_message(buff);
	if ((*victim).hp == 0) {
		(*victim).exist = 0;
		map_change[(*victim).pos.x][(*victim).pos.y] = 1;
		snprintf(buff, 100, "%s가 한줌의 모래로 돌아갔습니다.", (*victim).info_p->name);
		display_system_message(buff);
	}
}
void init_maps() {
	// init buildings
	for (int i = MAP_HEIGHT - 2; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < 2; j++) {
			building_map[i][j] = new_building(&b_base_info, (POSITION) { MAP_HEIGHT - 2, 0 });
		}
		for (int j = 2; j < 4; j++) {
			building_map[i][j] = new_building(&b_plate_info, (POSITION) { MAP_HEIGHT - 2, 2 });
		}
	}
	for (int i = 0; i < 2; i++) {
		for (int j = MAP_WIDTH - 4; j < MAP_WIDTH - 2; j++) {
			building_map[i][j] = new_building(&r_plate_info, (POSITION) { 0, MAP_WIDTH - 4 });
		}
		for (int j = MAP_WIDTH - 2; j < MAP_WIDTH; j++) {
			building_map[i][j] = new_building(&r_base_info, (POSITION) { 0, MAP_WIDTH - 2 });
		}
	}
	building_map[MAP_HEIGHT - 5][0] = new_building(&spice_info, (POSITION) { MAP_HEIGHT - 5, 0 });
	building_map[4][MAP_WIDTH - 1] = new_building(&spice_info, (POSITION) { 4, MAP_WIDTH - 1 });

	// init units
	unit_map[MAP_HEIGHT - 3][0] = new_unit(&b_havester_info, (POSITION) { MAP_HEIGHT - 3, 0 });
	unit_map[2][MAP_WIDTH - 1] = new_unit(&r_havester_info, (POSITION) { 2, MAP_WIDTH - 1 });

	// sandworm
	unit_map[0][0] = new_unit(&sandworm_info, (POSITION) { 0, 0 });
	unit_map[MAP_HEIGHT - 1][MAP_WIDTH - 1] = new_unit(&sandworm_info, (POSITION) { MAP_HEIGHT - 1, MAP_WIDTH - 1 });

	// init nature
	for (int r = 0; r < MAP_HEIGHT; r++) {
		for (int c = 0; c < MAP_WIDTH; c++) {
			nature_map[r][c] = &desert_info;
		}
	}
	init_total_rock();



	// map_change 초기상태 1
	for (int r = 0; r < MAP_HEIGHT; r++) {
		for (int c = 0; c < MAP_WIDTH; c++) {
			map_change[r][c] = 1;
		}
	}
}


// select
char get_repr(POSITION pos) {
	if (eagle.pos.x == pos.x && eagle.pos.y == pos.y) {
		return eagle.repr;
	}
	else if (unit_map[pos.x][pos.y].exist) {
		return unit_map[pos.x][pos.y].info_p->repr;
	}
	else if (building_map[pos.x][pos.y].exist) {
		return building_map[pos.x][pos.y].info_p->repr;
	}
	else {
		return nature_map[pos.x][pos.y]->repr;
	}
}
void select1() {
	selection.pos = cursor;
	selection.repr = get_repr(cursor);
	dispaly_state_message(selection.pos);
	display_cmd_message(selection.pos);
}

// 샌드웜
bool set_sandworm_dest(UNIT* sandworm) { // 너비우선탐색으로 가장 가까운 유닛을 탐색하고, 목적지로 설정하는 함수. 
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			dist[i][j] = -1;
		}
	}

	int head = 0, tail = 0;
	Q[tail++] = (*sandworm).pos;
	dist[(*sandworm).pos.x][(*sandworm).pos.y] = 0;

	while (head != tail) {
		POSITION cur = Q[head++];

		for (int i = 1; i <= 4; i++) {
			POSITION ncur = pmove(cur, i);

			if (ncur.x < 0 || ncur.x >= MAP_HEIGHT || ncur.y < 0 || ncur.y >= MAP_WIDTH || dist[ncur.x][ncur.y] != -1) continue;
			if (building_map[ncur.x][ncur.y].exist || nature_map[ncur.x][ncur.y]->repr == 'R') continue;
			if (unit_map[ncur.x][ncur.y].exist) {
				if (unit_map[ncur.x][ncur.y].info_p->repr == 'W') continue;
				else {
					(*sandworm).dest = ncur;
					return 1;
				}
			}

			dist[ncur.x][ncur.y] = dist[cur.x][cur.y] + 1;
			Q[tail++] = ncur;
		}
	}
	return 0;
}
int find_min_dist(POSITION pos, POSITION dest) { // 너비우선탐색으로 목적지까지의 거리를 구하는 함수.
	if (pos.x == dest.x && pos.y == dest.y) return 0;

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			dist[i][j] = -1;
		}
	}

	int head = 0, tail = 0;
	Q[tail++] = pos;
	dist[pos.x][pos.y] = 0;

	while (head != tail) {
		POSITION cur = Q[head++];
		for (int i = 1; i <= 4; i++) {
			POSITION ncur = pmove(cur, i);

			if (ncur.x < 0 || ncur.x >= MAP_HEIGHT || ncur.y < 0 || ncur.y >= MAP_WIDTH || dist[ncur.x][ncur.y] != -1) continue;
			if (building_map[ncur.x][ncur.y].exist || nature_map[ncur.x][ncur.y]->repr == 'R' || \
				(unit_map[ncur.x][ncur.y].exist && unit_map[ncur.x][ncur.y].info_p->repr == 'W')) continue;
			if (ncur.x == dest.x && ncur.y == dest.y) {
				return dist[cur.x][cur.y] + 1;
			}

			dist[ncur.x][ncur.y] = dist[cur.x][cur.y] + 1;
			Q[tail++] = ncur;
		}
	}
	return -1; // 목적지에 샌드웜이 있을 경우 
}
DIRECTION get_min_dir(POSITION pos, POSITION dest) { // 현재 위치에서 상하좌우 네방향중 어느방향으로 가는게 가장 최단거리인지 구함. 
	int min_dist = INT_MAX;
	DIRECTION dir = 0;

	for (DIRECTION d = 1; d <= 4; d++) {
		POSITION next_pos = pmove(pos, d);
		int nx = next_pos.x;
		int ny = next_pos.y;
		if (nx < 0 || nx >= MAP_HEIGHT || ny < 0 || ny >= MAP_WIDTH) continue;
		if (building_map[nx][ny].exist || nature_map[nx][ny]->repr == 'R' || (unit_map[nx][ny].exist && unit_map[nx][ny].info_p->repr == 'W')) continue;

		int tmp = find_min_dist(next_pos, dest); // 목적지까지의 거리를 반환 
		if (tmp < min_dist) { // 가장 짧은 거리를 저장
			min_dist = tmp;
			dir = d;
		}
	}

	if (min_dist == -1)
		return d_stay;
	else
		return dir;
}
void sandworm_move(UNIT* sandworm) {
	if (sys_clock < (*sandworm).next_move_time) return;
	(*sandworm).next_move_time = sys_clock + (*sandworm).info_p->move_period;

	unit_exist = set_sandworm_dest(sandworm);

	if (!unit_exist && (*sandworm).pos.x == (*sandworm).dest.x && (*sandworm).pos.y == (*sandworm).dest.y) {
		int nx, ny;
		do {
			nx = rand() % (MAP_HEIGHT - 2) + 1;
			ny = rand() % (MAP_WIDTH - 2) + 1;
		} while (building_map[nx][ny].exist || (unit_map[nx][ny].exist && unit_map[nx][ny].info_p->repr == 'W') || nature_map[nx][ny]->repr == 'R');

		POSITION new_pos = { nx, ny };
		(*sandworm).dest = new_pos;
	}

	DIRECTION dir = get_min_dir((*sandworm).pos, (*sandworm).dest);
	POSITION next_pos = pmove((*sandworm).pos, dir);
	if (unit_exist && next_pos.x == (*sandworm).dest.x && next_pos.y == (*sandworm).dest.y) {
		if (sys_clock >= (*sandworm).next_attack_time) {
			attack_unit(&sandworm_info, &unit_map[next_pos.x][next_pos.y]);
		}
		else {
			display_system_message("샌드웜이 공격을 준비중입니다!");
		}
	}
	else {
		unit_move((*sandworm).pos, next_pos);
	}
}
void eagle_move(SKY* eagle) {
	if (sys_clock < (*eagle).next_move_time) return;
	(*eagle).next_move_time = sys_clock + (*eagle).move_period;

	POSITION diff = psub((*eagle).dest, (*eagle).pos);
	DIRECTION dir;

	if ((*eagle).pos.x == (*eagle).dest.x && (*eagle).pos.y == (*eagle).dest.y) {
		if ((*eagle).dest.x == MAP_HEIGHT - 1)
			(*eagle).dest.x = 0;
		else
			(*eagle).dest.x++;

		(*eagle).dest.y = MAP_WIDTH - 1 - (*eagle).dest.y;
		dir = d_stay;
	}
	else if (abs(diff.x) >= abs(diff.y)) {
		dir = (diff.x >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.y >= 0) ? d_right : d_left;
	}

	POSITION next_pos = pmove((*eagle).pos, dir);

	map_change[(*eagle).pos.x][(*eagle).pos.y] = 1;
	map_change[next_pos.x][next_pos.y] = 1;
	(*eagle).pos = next_pos;

}

// 유닛 이동
void units_move() {
	bool exist[MAP_HEIGHT][MAP_WIDTH] = { 0 };
	for (int r = 0; r < MAP_HEIGHT; r++) {
		for (int c = 0; c < MAP_WIDTH; c++) {
			exist[r][c] = unit_map[r][c].exist;
		}
	}

	for (int r = 0; r < MAP_HEIGHT; r++) {
		for (int c = 0; c < MAP_WIDTH; c++) {
			if (exist[r][c]) {
				switch (unit_map[r][c].info_p->repr) {
				case 'W': sandworm_move(&unit_map[r][c]); break;

				}
			}
		}
	}

	eagle_move(&eagle);
}

// 유닛 생산
POSITION get_position(POSITION pos) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			dist[i][j] = -1;
		}
	}

	int head = 0, tail = 0;
	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			Q[tail++] = padd(building_map[pos.x][pos.y].pos, (POSITION) { r, c });
			dist[r][c] = 0;
		}
	}

	while (head != tail) {
		POSITION cur = Q[head++];
		for (int dir = 1; dir < 4; dir++) {
			POSITION ncur = pmove(cur, dir);

			if (ncur.x < 0 || ncur.x >= MAP_HEIGHT || ncur.y < 0 || ncur.y >= MAP_WIDTH || dist[ncur.x][ncur.y] != -1) continue;
			if (unit_map[ncur.x][ncur.y].exist) continue;
			if (building_map[ncur.x][ncur.y].exist && building_map[ncur.x][ncur.y].info_p->repr != 'P') continue;

			return ncur;
		}
	}
	return (POSITION) { 0, MAP_HEIGHT - 1 };
}
void make_unit(UNIT_INFO* unit_info, char repr) {
	if (selection.repr != repr) return;

	POSITION pos = get_position(selection.pos);
	if (pos.x == 0 && pos.y == MAP_HEIGHT - 1) {
		display_system_message("유닛을 생산할 공간이 부족합니다.");
		return;
	}
	if (resource.spice - unit_info->cost <= 0) {
		display_system_message("스파이스가 부족합니다.");
		return;
	}
	if (resource.population + unit_info->population >= resource.population_max) {
		display_system_message("인구수가 부족합니다.");
		return;
	}

	resource.spice -= unit_info->cost;
	resource.population += unit_info->population;

	unit_map[pos.x][pos.y] = new_unit(&b_havester_info, pos);
	map_change[pos.x][pos.y] = 1;
	char buff[100];
	snprintf(buff, 100, "%s 생산완료.", unit_info->name);
	display_system_message(buff);
}


/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	intro(); 
	init_maps();
	display_resource(resource);
	display_frame();
	display_map(cursor);

	while (1) {
		KEY key = get_key();

		if (vist_timer != -1 && vist_timer > 80) { // 80ms 이내에 다시 클릭이 이뤄지지 않았을때
			cursor_move(ktod(prev_key), 1);
		}
		// 방향키 입력이 있으면 처리
		if (is_arrow_key(key)) {
			if (vist_timer == -1) {
				vist_timer = 0;
				prev_key = key;
			}
			else {
				if (prev_key == key)
					cursor_move(ktod(key), double_click_distance);
				else
					cursor_move(ktod(key), 1);
			}
		}
		else {
			// 방향키 외의 입력
			switch (key) {
			case k_quit: outro();
			case k_space: select1(); break;
			case k_re_dis: re_display(resource, cursor, map_change); break;
			case k_esc: esc(&selection); break;
			case k_h: make_unit(&b_havester_info, 'B'); break;
			case k_none:
			case k_undef:
			default: break;
			}
		}


		units_move();
		display_map(cursor);
		display_resource(resource);
		display_time();


		Sleep(TICK);
		sys_clock += 10;
		if (vist_timer != -1) vist_timer += 10;
	}
}

// setting
void intro(void) {
	POSITION pos = { 10, 53 };
	gotoxy(pos);
	printf("DUNE 1.5\n");

	pos.x += 2;
	pos.y = 27;
	gotoxy(pos);

	printf("화면이 깨진다면 맵 크기를 줄이거나 창을 전체화면으로 설정해 주세요.\n");

	pos.x += 2;
	pos.y = 40;
	gotoxy(pos);
	printf("r을 입력하면 화면이 다시 출력됩니다.\n");

	pos.x += 5;
	pos.y = 38;
	gotoxy(pos);

	printf("더블 클릭시 이동할 거리를 입력해주세요 : ");
	scanf_s("%d", &double_click_distance);
	system("cls");
}
void outro(void) {
	set_color(COLOR_DEFAULT);
	system("cls");
	printf("exiting...\n");
	exit(0);
}

// 커서 이동
void cursor_move(DIRECTION dir, int n) { // 방향, 움직일 칸수
	vist_timer = -1;
	if (n == 0) {
		return;
	}

	POSITION curr = cursor;
	POSITION new_pos = pmove(curr, dir);
	// validation check
	if (0 <= new_pos.x && new_pos.x < MAP_HEIGHT && \
		0 <= new_pos.y && new_pos.y < MAP_WIDTH) {

		map_change[cursor.x][cursor.y] = 1;
		cursor = new_pos;
		map_change[cursor.x][cursor.y] = 1;

		cursor_move(dir, n - 1);
	}
}

