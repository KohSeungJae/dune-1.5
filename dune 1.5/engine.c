#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

/* ================= control =================== */
int sys_clock = 0;
POSITION cursor = { 0, 0 };
RESOURCE resource = {
	.spice = 100,
	.spice_max = 100,
	.population = 5,
	.population_max = 100
};
SELECTION selection = { 0 };


/* ================= game data =================== */
NATURE_INFO* map[MAP_HEIGHT][MAP_WIDTH];
SANDWORM sandworm[2];
UNIT units[100] = { 0 }; // 유닛 연결 리스트
BUILDING buildings[100] = { 0 }; // 빌딩 연결 리스트
int unit_unused = 1;
int building_unused = 1;
bool map_change[MAP_HEIGHT][MAP_WIDTH] = { 0 };


UNIT_INFO
error_n = {
	.repr = 'u',
	.name = "유닛 에러",
	.color = COLOR_RED,
	.cost = 5,
	.population = 5,
	.max_hp = 70,
	.move_period = 2000,
	.state_message = {
		.size = 1,
		.about_size = 1,
		.message = {
			"유닛 에러"
		}
	},
	.cmd_message = {
		.size = 1,
		.message = {
			"유닛 에러"
		}
	}
},
b_havester_info = {
	.repr = 'H',
	.name = "하베스터",
	.mother = 'B',
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
error_b = {
	.repr = 'b',
	.name = "건물 에러",
	.color = COLOR_BLUE,
	.cost = 0,
	.size = 2,
	.max_hp = 50,
	.state_message = {
		.size = 1,
		.about_size = 1,
		.message = {
			"건물 에러"
		}
	},
	.cmd_message = {
		.size = 1,
		.message = {
			"건물 에러"
		}
	}
},
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


EAGLE eagle = {
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
STORM storm = {
	.exist = 0,
	.pos = {MAP_HEIGHT - 2, MAP_WIDTH / 2},
	.repr = "STOM",
	.color = COLOR_DY,
	.next_move_time = 5000,
	.exist_time = 5000,
	.state_message = {
		.size = 3,
		.about_size = 3,
		.message = {
			"[모래폭풍]",
			"사나운 모래폭풍",
			"주기적으로 생성된다."
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
bool storm_on = 1;

// 유닛 & 빌딩을 연결리스트에 추가
void unit_push(UNIT_INFO* info, POSITION pos);
void unit_erase(POSITION pos);
void building_push(BUILDING_INFO* info, POSITION pos);
void building_erase(POSITION pos);
// init
inline void init_rock(POSITION pos);
inline void init_4rock(POSITION pos);
inline void init_total_rock();
void init();
// 커서 이동
void cursor_move(DIRECTION dir, int n);
// 선택
char get_repr(POSITION pos);
void select1();
// 유닛공격
void attack_unit(UNIT_INFO* attacker, UNIT* victim);
// 샌드웜 관련
bool set_sandworm_dest(SANDWORM* sandworm);
int find_min_dist(POSITION pos, POSITION dest);
DIRECTION get_min_dir(POSITION pos, POSITION dest);
void sandworm_move(SANDWORM* sandworm);
void sandworm_emission(SANDWORM* sandworm);
// 유닛 생성 함수
POSITION get_position(POSITION building_pos);
void make_unit(UNIT_INFO* unit_info);
// 독수리&모래폭풍
void eagle_move(EAGLE* eagle);
void storm_move();
void storm_move_test();
void storm_action(); 
// 객체 이동
inline void units_move();
// 인트로 & 종료
void intro();
void outro();
// test
void test_message() {
	display_system_message("test message");
}

int main(void) {
	srand((unsigned int)time(NULL));
	intro();
	init();
	display();

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
			case k_space: select1(); break;
			case k_h: make_unit(&b_havester_info); break;
			case k_re_dis: re_display(); break;
			case k_esc: esc(&selection); break;
			case k_quit: outro(); break;
			case k_test: test_message(); break;
			case k_none:
			case k_undef:
			default: break;
			}
		}

		units_move();
		display_time();
		display_map();

		Sleep(TICK);
		sys_clock += 10;
		if (vist_timer != -1) vist_timer += 10;
	}


	return 0;
}

// 인트로 & 종료
void intro() {
	gotoxy((POSITION) { 8, 55 });
	printf("DUNE 1.5");
	gotoxy((POSITION) { 12, 35 });
	printf("출력에 이상이 있을땐 r키를 누르면 화면이 재출력됩니다.");
	gotoxy((POSITION) { 15, 48 });
	printf("더블 클릭시 이동 거리 : ");
	scanf_s("%d", &double_click_distance);
	system("cls");
}
void outro() {
	char e;
	do {
		display_system_message("종료하시겠습니까? (Y / N) : ");
		bool poo = getchar();
		scanf_s("%c", &e, 1);
	} while (e != 'Y' && e != 'y' && e != 'n' && e != 'N');

	if (e == 'Y' || e == 'y') {
		display_system_message("게임을 종료합니다...");
		Sleep(1000);
		exit(0);
	}
	else {
		display_system_message("게임을 재개합니다.");
	}
}

// 유닛 & 빌딩을 연결리스트에 추가
void unit_push(UNIT_INFO* info, POSITION pos) {
	units[unit_unused].exist = 1;
	units[unit_unused].pos = pos;
	units[unit_unused].hp = info->max_hp;
	units[unit_unused].info_p = info;

	// 비어있는 칸
	int idx = 0;
	while (units[idx].exist) {
		idx++;
	}

	units[unit_unused].next = idx;
	units[idx].pre = unit_unused;
	unit_unused = idx;

	// 
	map_change[pos.x][pos.y] = 1;
}
void unit_erase(POSITION pos) { // 해당 위치의 유닛 삭제
	int idx = 0;
	while (units[idx].exist) {
		if (units[idx].pos.x == pos.x && units[idx].pos.y == pos.y) break;
		idx = units[idx].next;
	}
	if (idx == 0 || !units[idx].exist) return;

	int pre = units[idx].pre;
	int next = units[idx].next;

	units[idx].exist = 0;
	units[pre].next = next;
	units[next].pre = pre;

	map_change[pos.x][pos.y] = 1;
}

void building_push(BUILDING_INFO* info, POSITION pos) {
	buildings[building_unused].exist = 1;
	buildings[building_unused].pos = pos;
	buildings[building_unused].hp = info->max_hp;
	buildings[building_unused].info_p = info;

	// 비어있는 칸
	int idx = 0;
	while (buildings[idx].exist) {
		idx++;
	}

	buildings[building_unused].next = idx;
	buildings[idx].pre = building_unused;
	building_unused = idx;
}
void building_erase(POSITION pos) { // 해당 위치의 유닛 삭제 
	int idx = 0;
	while (buildings[idx].exist) {
		if (buildings[idx].pos.x == pos.x && buildings[idx].pos.y == pos.y) break;
		idx = buildings[idx].next;
	}
	if (idx == 0 || !buildings[idx].exist) return;

	int pre = buildings[idx].pre;
	int next = buildings[idx].next;

	buildings[idx].exist = 0;
	buildings[pre].next = next;
	buildings[next].pre = pre;
}

// init
inline void init_rock(POSITION pos) {
	map[pos.x][pos.y] = &rock_info;
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
void init() {
	units[0].exist = 1;
	units[0].next = 1;
	units[0].info_p = &error_n;
	units[1].pre = 0;
	buildings[0].exist = 1;
	buildings[0].next = 1;
	buildings[0].info_p = &error_b;
	buildings[1].pre = 0;

	// init unit
	unit_push(&b_havester_info, (POSITION) { MAP_HEIGHT - 3, 0 });
	unit_push(&r_havester_info, (POSITION) { 2, MAP_WIDTH - 1 });
	// init building
	building_push(&b_base_info, (POSITION) { MAP_HEIGHT - 2, 0 });
	building_push(&r_base_info, (POSITION) { 0, MAP_WIDTH - 2 });
	building_push(&b_plate_info, (POSITION) { MAP_HEIGHT - 2, 2 });
	building_push(&r_plate_info, (POSITION) { 0, MAP_WIDTH - 4 });
	building_push(&spice_info, (POSITION) { MAP_HEIGHT - 5, 0 });
	building_push(&spice_info, (POSITION) { 4, MAP_WIDTH - 1 });


	sandworm[0].pos = (POSITION){ 0,0 };
	sandworm[1].pos = (POSITION){ MAP_HEIGHT - 1, MAP_WIDTH - 1 };
	for (int i = 0; i < 2; i++) {
		sandworm[i].hp = sandworm_info.max_hp;
		sandworm[i].len = 1;
		sandworm[i].info_p = &sandworm_info;
		sandworm[i].next_move_time = sandworm_info.move_period;
		sandworm[i].next_attack_time = sandworm_info.attack_period;
		sandworm[i].next_emission_time = 20000;
	}


	for (int r = 0; r < MAP_HEIGHT; r++) {
		for (int c = 0; c < MAP_WIDTH; c++) {
			map[r][c] = &desert_info;
		}
	}
	init_total_rock();

	// cursor
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

// 선택
char get_repr(POSITION pos) { // 해당 위치에 있는 객체의 문자를 가져오는 함수. 
	int idx = get_unit_idx(pos);
	if (idx) {
		return units[idx].info_p->repr;
	}
	idx = get_sandworm_idx(pos);
	if (idx != 3) {
		return sandworm[idx].info_p->repr;
	}
	idx = get_building_idx(pos);
	if (idx) {
		return buildings[idx].info_p->repr;
	}
	return map[pos.x][pos.y]->repr;
}
void select1() { // 스페이스바 입력시, 해당 위치와 해당 객체의 문자를 저장, 상태창, 명령창 출력.
	selection.pos = cursor;
	selection.repr = get_repr(cursor);
	display_state_message(selection.pos);
	display_cmd_message(selection.pos);
}

// 유닛 공격
void attack_unit(UNIT_INFO* attacker, UNIT* victim) {
	(*victim).hp = ((*victim).hp - attacker->damage <= 0) ? 0 : (*victim).hp - attacker->damage;

	char buff[100];
	snprintf(buff, 100, "%s가 %s에게 공격당했습니다. (남은체력 : %d)", (*victim).info_p->name, attacker->name, (*victim).hp);
	display_system_message(buff);
	if ((*victim).hp == 0) {
		unit_erase((*victim).pos);
		snprintf(buff, 100, "%s가 한줌의 모래로 돌아갔습니다.", (*victim).info_p->name);
		display_system_message(buff);
	}
}

// 샌드웜 관련 함수
bool set_sandworm_dest(SANDWORM* sandworm) { // 너비우선탐색으로 가장 가까운 유닛을 탐색하고, 목적지로 설정. 
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			dist[i][j] = -1;
		}
	}

	int head = 0, tail = 0;
	Q[tail++] = (*sandworm).pos;
	dist[(*sandworm).pos.x][(*sandworm).pos.y] = 0;

	while (head != tail) {
		POSITION pos = Q[head++];

		for (int i = 1; i <= 4; i++) {
			POSITION npos = pmove(pos, i);

			if (npos.x < 0 || npos.x >= MAP_HEIGHT || npos.y < 0 || npos.y >= MAP_WIDTH || dist[npos.x][npos.y] != -1) continue;
			if (get_building_idx(npos) || map[npos.x][npos.y]->repr == 'R') continue;
			if (get_unit_idx(npos)) {
				if (get_sandworm_idx(npos) != 3) continue;
				else {
					(*sandworm).dest = npos;
					return 1;
				}
			}

			dist[npos.x][npos.y] = dist[pos.x][pos.y] + 1;
			Q[tail++] = npos;
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
		POSITION pos = Q[head++];
		for (int i = 1; i <= 4; i++) {
			POSITION npos = pmove(pos, i);

			if (npos.x < 0 || npos.x >= MAP_HEIGHT || npos.y < 0 || npos.y >= MAP_WIDTH || dist[npos.x][npos.y] != -1) continue;
			if (map[npos.x][npos.y]->repr == 'R' || get_sandworm_idx(npos) != 3 || \
				get_building_idx(npos)) continue;
			if (npos.x == dest.x && npos.y == dest.y) {
				return dist[pos.x][pos.y] + 1;
			}

			dist[npos.x][npos.y] = dist[pos.x][pos.y] + 1;
			Q[tail++] = npos;
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
		if (map[nx][ny]->repr == 'R' || get_sandworm_idx(next_pos) != 3 || \
			get_building_idx(next_pos))continue;

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
void sandworm_move(SANDWORM* sandworm) {
	if (sys_clock < (*sandworm).next_move_time) return;
	(*sandworm).next_move_time = sys_clock + (*sandworm).info_p->move_period;

	// 목적지 설정
	unit_exist = set_sandworm_dest(sandworm);

	if (!unit_exist && (*sandworm).pos.x == (*sandworm).dest.x && (*sandworm).pos.y == (*sandworm).dest.y) {
		int nx, ny;
		do {
			nx = rand() % (MAP_HEIGHT - 2) + 1;
			ny = rand() % (MAP_WIDTH - 2) + 1;
		} while (map[nx][ny]->repr == 'R' || get_sandworm_idx((POSITION) { nx, ny }) != 3 || \
			get_building_idx((POSITION) { nx, ny }));

		POSITION new_pos = { nx, ny };
		(*sandworm).dest = new_pos;
	}

	// 방향 설정
	DIRECTION dir = get_min_dir((*sandworm).pos, (*sandworm).dest);
	POSITION next_pos = pmove((*sandworm).pos, dir);
	if (unit_exist && next_pos.x == (*sandworm).dest.x && next_pos.y == (*sandworm).dest.y) {
		if (sys_clock >= (*sandworm).next_attack_time) {
			(*sandworm).next_attack_time = sys_clock + sandworm_info.attack_period;
			int idx = get_unit_idx(next_pos);
			attack_unit(&sandworm_info, &units[idx]);
		}
		else { // 유닛 앞에 도착했지만 공격주기가 되지 않았을경우.
			display_system_message("샌드웜이 공격을 준비중입니다!");
		}
	}
	else {
		map_change[(*sandworm).pos.x][(*sandworm).pos.y] = 1;
		(*sandworm).pos = next_pos;
		map_change[(*sandworm).pos.x][(*sandworm).pos.y] = 1;
	}
}
void sandworm_emission(SANDWORM* sandworm) {
	if (sys_clock < (*sandworm).next_emission_time) return;
	(*sandworm).next_emission_time += (rand() % 300 + 100) * 1000;
	building_push(&spice_info, (*sandworm).pos);
	int idx = get_building_idx((*sandworm).pos);
	buildings[idx].hp = rand() % 9 + 1;
	char buff[100];
	snprintf(buff, 100, "샌드웜이 스파이스를 배출했습니다.(매장량 : %d)", buildings[idx].hp);
	display_system_message(buff);
	snprintf(buff, 100, "다음 배출 시간 : %d : %d : %d",
		(*sandworm).next_emission_time / 60000,
		((*sandworm).next_emission_time / 1000) % 60,
		((*sandworm).next_emission_time / 10) % 60);
	display_system_message(buff);
}

// 유닛 생성 함수
POSITION get_position(POSITION building_pos) { // 유닛을 생성할때 생성위치를 반환
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			dist[i][j] = -1;
		}
	}

	int idx = get_building_idx(building_pos);
	int head = 0, tail = 0;
	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			Q[tail++] = padd(buildings[idx].pos, (POSITION) { r, c });
			dist[r][c] = 0;
		}
	}

	while (head != tail) {
		POSITION cur = Q[head++];
		for (int dir = 1; dir <= 4; dir++) {
			POSITION npos = pmove(cur, dir);

			if (npos.x < 0 || npos.x >= MAP_HEIGHT || npos.y < 0 || npos.y >= MAP_WIDTH || dist[npos.x][npos.y] != -1) continue;
			if (get_unit_idx(npos)) continue;
			if (get_building_idx(npos)) continue;

			return npos;
		}
	}
	return (POSITION) { 0, MAP_HEIGHT - 1 };
}
void make_unit(UNIT_INFO* unit_info) { // 유닛을 올바른 위치에 추가하는 함수
	int idx = get_building_idx(selection.pos);
	if (buildings[idx].info_p->repr != unit_info->mother || buildings[idx].info_p->color == COLOR_RED) return;

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

	unit_push(unit_info, pos);
	char buff[100];
	snprintf(buff, 100, "%s 생산완료.", unit_info->name);
	display_system_message(buff);
}

// 독수리&모래폭풍
void eagle_move(EAGLE* eagle) {
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
void storm_move() {
	if (!storm.exist && sys_clock < storm.exist_time) { // 생성시간이 되지 않음.
		return;
	}

	if (!storm.exist && sys_clock >= storm.exist_time) { // 생성시간이 됨.
		storm.exist = 1;
		storm.exist_time = sys_clock + 5000; // 생성후 5000ms 동안 존재
		display_system_message("모래폭풍이 발생했습니다!");
		do {
			storm.pos.x = rand() % MAP_HEIGHT;
			storm.pos.y = rand() % (MAP_WIDTH - 16) + 8; // 본진 근처에는 생성되지 않음.
		} while (storm.pos.x < 0 || storm.pos.x >= MAP_HEIGHT - 1 || storm.pos.y < 0 || storm.pos.y >= MAP_WIDTH - 1);

		for (int r = 0; r < 2; r++) { // 맵 출력을 위해 map_change = 1 
			for (int c = 0; c < 2; c++) {
				POSITION pos = padd(storm.pos, (POSITION) { r, c });
				map_change[pos.x][pos.y] = 1;
			}
		}
	}

	if (storm.exist && sys_clock >= storm.exist_time) { // 생성 후, 소멸 시간이 됨.
		storm.exist = 0;
		storm.exist_time += 30000; // 30초 뒤 재생성
		storm.next_move_time += 30000;
		display_system_message("모래폭풍이 소멸했습니다.");

		for (int r = 0; r < 2; r++) { // 맵 출력을 위해 map_change = 1 
			for (int c = 0; c < 2; c++) {
				POSITION pos = padd(storm.pos, (POSITION) { r, c });
				map_change[pos.x][pos.y] = 1;
			}
		}
		return;
	}

	// 모래폭풍 이동
	if (sys_clock < storm.next_move_time) return;
	storm.next_move_time += 100;

	DIRECTION dir;
	POSITION n_pos;
	do {
		dir = rand() % 4 + 1;
		n_pos = pmove(storm.pos, dir);
	} while (n_pos.x < 0 || n_pos.x >= MAP_HEIGHT - 1 || n_pos.y < 0 || n_pos.y >= MAP_WIDTH - 1);


	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			POSITION pos = padd(storm.pos, (POSITION) { r, c });
			map_change[pos.x][pos.y] = 1;
		}
	}

	storm.pos = n_pos;

	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			POSITION pos = padd(storm.pos, (POSITION) { r, c });
			map_change[pos.x][pos.y] = 1;
		}
	}
}
void storm_move_test() {
	if (!storm.exist && sys_clock < storm.exist_time) { // 생성시간이 되지 않음.
		return;
	}

	if (!storm.exist && sys_clock >= storm.exist_time) { // 생성시간이 됨.
		storm.exist = 1;
		storm.exist_time = sys_clock + 5000; // 생성후 5000ms 동안 존재
		display_system_message("모래폭풍이 발생했습니다!");

		// 본진에 생성(파괴 테스트용)
		storm.pos.x = MAP_HEIGHT-3;
		storm.pos.y = 0; 


		for (int r = 0; r < 2; r++) { // 맵 출력을 위해 map_change = 1 
			for (int c = 0; c < 2; c++) {
				POSITION pos = padd(storm.pos, (POSITION) { r, c });
				map_change[pos.x][pos.y] = 1;
			}
		}
		return;
	}

	if (storm.exist && sys_clock >= storm.exist_time) { // 생성 후, 소멸 시간이 됨.
		storm.exist = 0;
		storm.exist_time += 30000; // 30초 뒤 재생성
		storm.next_move_time += 30000;
		display_system_message("모래폭풍이 소멸했습니다.");

		for (int r = 0; r < 2; r++) { // 맵 출력을 위해 map_change = 1 
			for (int c = 0; c < 2; c++) {
				POSITION pos = padd(storm.pos, (POSITION) { r, c });
				map_change[pos.x][pos.y] = 1;
			}
		}
		return;
	}

	// 모래폭풍 이동
	if (sys_clock < storm.next_move_time) return;
	storm.next_move_time += 100;

	DIRECTION dir;
	POSITION n_pos;
	do {
		dir = rand() % 4 + 1;
		n_pos = pmove(storm.pos, dir);
	} while (n_pos.x < 0 || n_pos.x >= MAP_HEIGHT - 1 || n_pos.y < 0 || n_pos.y >= MAP_WIDTH - 1);


	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			POSITION pos = padd(storm.pos, (POSITION) { r, c });
			map_change[pos.x][pos.y] = 1;
		}
	}

	storm.pos = n_pos;

	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			POSITION pos = padd(storm.pos, (POSITION) { r, c });
			map_change[pos.x][pos.y] = 1;
		}
	}
}
void storm_action() {
	if (!storm.exist) return; 

	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			POSITION pos = padd(storm.pos, (POSITION) { r, c });
			int idx = get_unit_idx(pos);  
			if (idx) { 
				char buff[100];
				snprintf(buff, 100, "%s이(가) 모래폭풍에 휩쓸렸습니다.", units[idx].info_p->name); 
				display_system_message(buff);   
				unit_erase(pos);
			}
		}
	}

	for (int r = 0; r < 2; r++) { 
		for (int c = 0; c < 2; c++) { 
			POSITION pos = padd(storm.pos, (POSITION) { r, c }); 
			int idx = get_building_idx(pos);
			if (idx && buildings[idx].info_p->repr != 'P' && buildings[idx].info_p->repr != 'S' && !buildings[idx].destroied) {
				char buff[100];
				snprintf(buff, 100, "%s이(가) 반파되었습니다.", buildings[idx].info_p->name);  
				display_system_message(buff);  
				buildings[idx].destroied = 1;
				buildings[idx].hp /= 2;
			}
		}
	}
}
// 유닛 이동
inline void units_move() {
	storm_action();
	for (int i = 0; i < 2; i++) {
		sandworm_emission(&sandworm[i]);
		sandworm_move(&sandworm[i]);
	}
	eagle_move(&eagle);
	storm_move();
	// storm_move_test();
}
