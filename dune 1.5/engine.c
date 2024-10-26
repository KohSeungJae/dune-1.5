#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"



void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir, int n);

// 유닛 함수(공용)
void eagle_move(); 


// 샌드웜 함수
void sand_worm_move();
void sand_worm_onoff();
void sand_worm_excretion(int x, int y);

// 행동
void object_action(KEY key);


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { 1,1 };


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };
int map_color[N_C_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };     // 지형, 건물의 색 저장, map과 마찬가지로 투영시켜서 출력

int spice_num[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // 스파이스의 매장량

POSITION Q[MAP_HEIGHT * MAP_WIDTH]; // BFS용 큐, 접근은 head 와 tail로 하기 때문에 쓰여진 값을 지우지 않아도 됨.
int dist[MAP_HEIGHT][MAP_WIDTH];	// 이거는 쓸때마다 -1로 초기화 해줘야함

RESOURCE resource = {
	.spice = 5,
	.spice_max = 5,
	.population = 5,
	.population_max = 100
};

// 유닛
OBJECT eagle = {
	.layer = 2,
	.pos = {2,25},
	.dest = {2, MAP_WIDTH - 2},	
	.repr = 'E',
	.color = COLOR_PURPLE,
	.speed = 100,
	.next_move_time = 100
}; 
OBJECT sand_worm[2] = {
	{
	.layer = 1,
	.pos = {1 , 1},
	.dest = {10,10},
	.repr = 'W',
	.len = 1,
	.color = COLOR_DY,
	.speed = 800,
	.next_move_time = 0,
	}
	,
	{
	.layer = 1,
	.pos = {MAP_HEIGHT-2 , MAP_WIDTH-2},
	.dest = {10,10},
	.repr = 'W',
	.len = 1,
	.color = COLOR_DY,
	.speed = 800,
	.next_move_time = 0,
	}
};
OBJECTS havester = { 
	.cost = 5,
	.population = 5,
	.mother = 'B',
	.size = 0,
	.layer = 1,
	.repr = 'H',
	.color = COLOR_BULE,
	.speed = 2000,
	.next_move_time = 0,
	.order_message = {
		.size = 2,
		.message = {
		"H: 스파이스 채집",
		"M: 이동"
		}
	},
	.state_message = {
		.size = 8,
		.about_size = 2,
		.message = {
			"[하베스터]",
			"스파이스를 수확해 배달한다.",
			"생산 비용 : 5",
			"인구수 : 5",
			"이동 주기 : 2000",
			"공격력 : 0",
			"체력 : 70",
			"시야 : 0",
		}
	},

	.object[0] = {
		.pos = {MAP_HEIGHT - 4, 1},
		.dest = {-1, -1},
		.hp = 70
	}
};

// 건물
STRUCTURE base = {
	.repr = 'B',
	.cost = 0,
	.size = 2,     // 한변의 길이
	.state_message = {
		.size = 2,
		.about_size = 3,
		.message = {
			"[본진]",
			"하베스터를 생산 할 수 있다."
		}
	},
	.order_message = {
		.size = 1,
		.message = {
			"H : 하베스터 생산"
		}
	},
	.structure[0] = {
		.hp = 50,
		.pos = {MAP_HEIGHT - 3, 1} // 좌상단 좌표
	}
};



int double_click_distance = 1;
int vist_timer = -1;
KEY prev_key; 
bool sand_worm_on = 1;
int object_num = 2;
int excretion_time = 6000;
char selection = -1;

/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();


	display(resource, map, cursor);
	display_frame();

	while (1) {
		KEY key = get_key();

		if (vist_timer != -1 && vist_timer > 80) { // 80ms 이내에 다시 클릭이 이뤄지지 않았을때
			cursor_move(ktod(prev_key), 1);
		}
		// 키 입력이 있으면 처리
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
			case k_re_dis: 
				re_display(resource, map, cursor); 
				break;
			case k_test:
				display_system_message("new strrrrrrr");
				break;
			case k_1:
				display_system_message("test");
				break;
			case k_w:
				sand_worm_onoff();
				break;
			case k_space:
				select_object(cursor); 
				break;
			case k_h:
				object_action(key); 
				break;
			case k_esc:
				esc();
				break;

			case k_none:
			case k_undef:
			default: break;
			}
		}

		// 샌드웜 동작
		if (sand_worm_on)
			sand_worm_move();

		// 독수리 이동
		eagle_move();

		// 화면 출력
		display(resource, map, cursor);
		
		Sleep(TICK);
		sys_clock += 10;
		if (vist_timer != -1) vist_timer += 10;
	}
}

// setting
void intro(void) {
	POSITION pos = {10, 53};
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

void init_rock(POSITION pos) {
	map[0][pos.x][pos.y] = 'R';
	map_color[0][pos.x][pos.y] = COLOR_GARY;
	map_color[0][pos.x][pos.y] = COLOR_GARY;
}

void init_4rock(POSITION pos) {
	for (int i = pos.x; i < pos.x + 2; i++) {
		for (int j = pos.y; j < pos.y + 2; j++) {
			POSITION pos = { i, j };
			init_rock(pos);
		}
	}
}

void init(void) {
	// map_color 초기화
	for (int k = 0; k < N_C_LAYER; k++) {
		for (int i = 0; i < MAP_HEIGHT; i++) {
			for (int j = 0; j < MAP_WIDTH; j++) {
				map_color[k][i][j] = -1;
			}
		}
	}
	// 커서
	map_color[3][1][1] = COLOR_CURSOR;

	// layer 0(map[0])에 지형 생성
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
		map_color[0][0][j] = COLOR_DEFAULT;
		map_color[0][MAP_HEIGHT -1][j] = COLOR_DEFAULT; 
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		map_color[0][i][0] = COLOR_DEFAULT;
		map_color[0][i][MAP_WIDTH - 1] = COLOR_DEFAULT;
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';
			map_color[0][i][j] = COLOR_YELLOW;  
		}
	}

	// 블루팀
	for (int i = MAP_HEIGHT - 3; i < MAP_HEIGHT - 1; i++) {
		for (int j = 1; j < 3; j++) {
			map[0][i][j] = 'B';
			map_color[0][i][j] = COLOR_BULE; 
			map_color[0][i][j] = COLOR_BULE;
		}
		for (int j = 3; j < 5; j++) {
			map[0][i][j] = 'P';
			map_color[0][i][j] = COLOR_BLACk;
			map_color[0][i][j] = COLOR_BLACk;
		}
	}
	
	// 레드팀
	for (int i = 1; i < 3; i++) {
		for (int j = MAP_WIDTH - 3; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = 'B';
			map_color[0][i][j] = COLOR_RED;
			map_color[0][i][j] = COLOR_RED;
		}
		for (int j = MAP_WIDTH - 5; j < MAP_WIDTH - 3; j++) {
			map[0][i][j] = 'P';
			map_color[0][i][j] = COLOR_BLACk;
			map_color[0][i][j] = COLOR_BLACk;
		}
	}

	// 스파이스
	map[0][MAP_HEIGHT - 6][1] = 'S'; // 블루팀 
	spice_num[MAP_HEIGHT - 6][1] = 9;
	map_color[0][MAP_HEIGHT - 6][1] = COLOR_DY; 
	map_color[0][MAP_HEIGHT - 6][1] = COLOR_DY;

	map[0][5][MAP_WIDTH - 2] = 'S';  // 레드팀 
	spice_num[5][MAP_WIDTH - 2] = 5;
	map_color[0][5][MAP_WIDTH - 2] = COLOR_DY; 
	map_color[0][5][MAP_WIDTH - 2] = COLOR_DY;

	

	// 돌
	POSITION pos[5] = { {5,16}, {MAP_HEIGHT - 7, MAP_WIDTH - 18},  { MAP_HEIGHT - 7,10 }, {6, MAP_WIDTH - 11}, {MAP_HEIGHT/2, MAP_WIDTH/2}}; 
	init_4rock(pos[0]);
	init_4rock(pos[1]);
	init_rock(pos[2]);
	init_rock(pos[3]);
	init_rock(pos[4]);
	/*
	POSITION pos1 = { 5, 1 }; 
	for (int i = 0; i < 5; i++) {
		init_rock(pos1); 
		pos1.y += 1;
	}
	pos1.x = 6;
	pos1.y = 3;
	init_rock(pos1);
	pos1.x = 7;
	pos1.y = 3;
	for (int i = 0; i < 5; i++) {
		init_rock(pos1);
		pos1.y += 1;
	}
	*/

	/*
	POSITION pos1 = {MAP_HEIGHT-3, MAP_WIDTH-3};
	init_4rock(pos1);
	/*/


	for (int l = 1; l < N_LAYER; l++) {
		for (int r = 0; r < MAP_HEIGHT; r++) {
			for (int c = 0; c < MAP_WIDTH; c++) {
				map[l][r][c] = -1;
			}
		}
	}

	// 유닛(1레이어) 
	// 하베스터
	map[1][MAP_HEIGHT - 4][1] = 'H';
	map_color[1][MAP_HEIGHT - 4][1] = COLOR_BULE; 

	map[1][3][MAP_WIDTH - 2] = 'H';
	map_color[1][3][MAP_WIDTH - 2] = COLOR_RED;

	// 유닛 추가
	// 독수리
	map[2][eagle.pos.x][eagle.pos.y] = eagle.repr;
	map_color[2][eagle.pos.x][eagle.pos.y] = eagle.color;

	// 샌드웜
	map[1][1][1] = 'W';
	map_color[1][1][1] = COLOR_DY;
	map[1][MAP_HEIGHT - 2][MAP_WIDTH - 2] = 'W'; 
	map_color[1][MAP_HEIGHT - 2][MAP_WIDTH - 2] = COLOR_DY;

	//모래폭풍

	map[2][10][20] = 's';
	map[2][10][21] = 'T';
	map[2][11][20] = 'O';
	map[2][11][21] = 'm';
	map_color[2][10][20] = COLOR_DY; 
	map_color[2][10][21] = COLOR_DY; 
	map_color[2][11][20] = COLOR_DY; 
	map_color[2][11][21] = COLOR_DY;

}


// 커서 이동
void cursor_move(DIRECTION dir, int n) { // 방향, 움직일 칸수
	if (n == 0) {
		vist_timer = -1;
		return;
	}

	POSITION curr = cursor; 
	POSITION new_pos = pmove(curr, dir); 
	// validation check
	if (1 <= new_pos.x && new_pos.x <= MAP_HEIGHT - 2 && \
		1 <= new_pos.y && new_pos.y <= MAP_WIDTH - 2) {
		map_color[3][cursor.x][cursor.y] = -1;
		cursor = new_pos;
		map_color[3][cursor.x][cursor.y] = COLOR_CURSOR; 
	}
	cursor_move(dir, n - 1);  
}

// 독수리 이동
void eagle_move() {
	if (sys_clock <= eagle.next_move_time) return; 
	POSITION diff = psub(eagle.dest, eagle.pos);
	DIRECTION dir;

	if (eagle.pos.x == eagle.dest.x && eagle.pos.y == eagle.dest.y) {
		if (eagle.dest.x == MAP_HEIGHT - 2)
			eagle.dest.x = 1;
		else
			eagle.dest.x++;

		eagle.dest.y = MAP_WIDTH-1 - eagle.dest.y;
		dir = d_stay; 
	}
	else if (abs(diff.x) >= abs(diff.y)) {
		dir = (diff.x >= 0) ? d_down : d_up;

	}
	else {
		dir = (diff.y >= 0) ? d_right : d_left;

	}

	int r = eagle.pos.x;
	int c = eagle.pos.y;

	POSITION next_pos = pmove(eagle.pos, dir); 
	int nr = next_pos.x;
	int nc = next_pos.y;

	if (next_pos.x < 1 || next_pos.x > MAP_HEIGHT - 2 || \
		 next_pos.y < 1 || next_pos.y > MAP_WIDTH - 2) { // 맵을 벗어날때
		return; 
	}
	else {
		map[2][r][c] = -1; 
		map_color[2][r][c] = -1;
		map[2][nr][nc] = 'E';
		map_color[2][nr][nc] = COLOR_PURPLE;
		eagle.pos = next_pos; 
	}

	eagle.next_move_time = sys_clock + eagle.speed;
}

// 샌드웜 관련 함수
void sand_worm_onoff() {
	if (sand_worm_on) {
		sand_worm_on = 0;
		display_system_message("샌드웜 비활성화");
	}
	else {
		sand_worm_on = 1;
		display_system_message("샌드웜 활성화");
	}
}

void set_sand_worm_dest(int idx) { // 너비우선탐색으로 가장 가까운 유닛을 탐색하고, 목적지로 설정하는 함수.
	for (int i = 0; i < MAP_HEIGHT; i++) { 
		for (int j = 0; j < MAP_WIDTH; j++) { 
			dist[i][j] = -1; 
		}
	}

	int head = 0, tail = 0; 
	Q[tail++] = sand_worm[idx].pos;
	dist[sand_worm[idx].pos.x][sand_worm[idx].pos.y] = 0;
	bool find_dest = 0; 

	while (head != tail) {
		POSITION cur = Q[head++]; 

		for (int i = 1; i <= 4; i++) {
			POSITION ncur = pmove(cur, i); 

			if (ncur.x < 1 || ncur.x > MAP_HEIGHT - 2 || ncur.y < 1 || ncur.y > MAP_WIDTH - 2) continue;
			if (map[0][ncur.x][ncur.y] != ' ' || dist[ncur.x][ncur.y] != -1) continue;
			if (map[1][ncur.x][ncur.y] != 'W' && (map[1][ncur.x][ncur.y] != -1 || object_num == 0)) {
				sand_worm[idx].dest = ncur;
				find_dest = 1;
			}

			dist[ncur.x][ncur.y] = dist[cur.x][cur.y] + 1;
			Q[tail++] = ncur;
		}
		if (find_dest) break;
	}
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
	bool find_dest = 0;

	while (head != tail) { 
		POSITION cur = Q[head++]; 
		for (int i = 1; i <= 4; i++) {
			POSITION ncur = pmove(cur, i); 

			if (ncur.x < 1 || ncur.x > MAP_HEIGHT - 2 || ncur.y < 1 || ncur.y > MAP_WIDTH - 2) continue; 
			if (map[0][ncur.x][ncur.y] != ' ' || dist[ncur.x][ncur.y] != -1) continue; 
			if (ncur.x == dest.x &&  ncur.y == dest.y) { 
				find_dest = 1; 
				return dist[cur.x][cur.y] + 1; 
			}

			dist[ncur.x][ncur.y] = dist[cur.x][cur.y] + 1;
			Q[tail++] = ncur;
		}
		if (find_dest) break;
	}
	return 0;
}

POSITION sand_worm_next_position(int idx) {
	// 샌드웜 목적지 설정
	if (object_num != 0)
		set_sand_worm_dest(idx);

	// 남은 유닛이 없을때 && 샌드웜이 목적지에 도착했을때 -> 랜덤 목적지
	if (object_num == 0 &&\
		(sand_worm[idx].pos.x == sand_worm[idx].dest.x) &&\
		(sand_worm[idx].pos.y == sand_worm[idx].dest.y)) { 
		int nx, ny;
		do {
			nx = rand() % (MAP_HEIGHT - 2) + 1;
			ny = rand() % (MAP_WIDTH - 2) + 1;
		} while (map[0][nx][ny] != ' ');

		POSITION new_pos = { nx, ny };
		sand_worm[idx].dest = new_pos;
	}

	int min_dist = INT_MAX; 
	DIRECTION dir; 
	// 현재 위치에서 상하좌우 네방향중 어느방향으로 가는게 가장 최단거리인지 구함.
	for (DIRECTION d = 1; d <= 4; d++) { 
		POSITION next_pos = pmove(sand_worm[idx].pos, d);
		int nx = next_pos.x;
		int ny = next_pos.y;
		if (nx < 1 || nx > MAP_HEIGHT - 2 || ny < 1 || ny > MAP_WIDTH - 2 || map[0][nx][ny] != ' ') continue; 
		
		int tmp = find_min_dist(next_pos, sand_worm[idx].dest); // 목적지까지의 거리를 반환
		if (tmp < min_dist) { 
			min_dist = tmp; 
			dir = d;  
		}
	}

	POSITION next_pos = pmove(sand_worm[idx].pos, dir);
	int nx = next_pos.x;
	int ny = next_pos.y;

	if (nx < 1 || nx > MAP_HEIGHT - 2 || ny < 1 || ny > MAP_WIDTH - 2) { // 맵을 벗어날때
		return sand_worm[idx].pos;
	}
	else if (map[0][nx][ny] != ' ') { 
		display_system_message("샌드웜 에러: 이동위치에 장애물이 있습니다."); // 오류 확인용
		sand_worm_onoff();
		return next_pos;
	}
	else {
		return next_pos;  // 다음 위치
	}
}

void sand_worm_move() {
	for (int i = 0; i < 2; i++) {
		if (sys_clock <= sand_worm[i].next_move_time) return;
		if (sand_worm[i].pos.x == sand_worm[i].dest.x && sand_worm[i].pos.y == sand_worm[i].dest.y && object_num != 0) {
			object_num--;
			sand_worm[i].len++;
			display_system_message("샌드웜이 유닛을 잡아먹었습니다.");
			/*
			char buffer[20];
			snprintf(buffer, sizeof(buffer), "샌드웜의 길이 : %d", sand_worm[i].len);
			display_system_message(buffer);
			*/
		}


		int x = sand_worm[i].pos.x;
		int y = sand_worm[i].pos.y;

		// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
		map[1][x][y] = -1; // 유닛 지나간 자리 복구
		map_color[1][x][y] = -1;
		sand_worm_excretion(x, y);

		// 움직일 자리
		sand_worm[i].pos = sand_worm_next_position(i); 
		x = sand_worm[i].pos.x;
		y = sand_worm[i].pos.y;

		// 같은레이어의 유닛이 겹치면 하나가 지워져버림 
		map[1][x][y] = sand_worm[i].repr;
		map_color[1][x][y] = sand_worm[i].color; // 배경색상 변경 

		sand_worm[i].next_move_time = sys_clock + sand_worm[i].speed;
	}
}

void sand_worm_excretion(int x, int y) {
	if (excretion_time >= sys_clock) return;  
	int next_time = (rand() % 180 + 60) * 1000; // 1분 - 3분 사이
	excretion_time += next_time; 


	map[0][x][y] = 'S';
	map_color[0][x][y] = COLOR_DY;
	spice_num[x][y] = rand() % 9 + 1;

	char buffer[100]; 
	snprintf(buffer, 100, "샌드웜이 스파이스를 배출했습니다. 매장량 : %d", spice_num[x][y]); 
	display_system_message(buffer); 
	snprintf(buffer, 100, "다음 배출 %d:%d",  (sys_clock + next_time) / 60000, ((sys_clock + next_time) / 1000 ) % 60);
	display_system_message(buffer);  
}


// 행동
void object_action(KEY key) {
	OBJECTS* cur_obj = &(havester);
	switch (key) {
	case 'H': cur_obj = &(havester); break;
	}


	if ((*cur_obj).mother != selection) return;
	if (resource.spice < (*cur_obj).cost) {
		display_system_message("스파이스가 부족합니다.");
		return;
	}
	if (resource.population + (*cur_obj).population > resource.population_max) {
		display_system_message("인구수가 부족합니다.");
		return;
	}

	char buffer[100];
	snprintf(buffer, 100, "%s 생성완료.", (*cur_obj).state_message.message[0]);
	display_system_message(buffer);

	int i = ++(*cur_obj).size;
	int x = MAP_HEIGHT - 4;
	int y = 2;
	(*cur_obj).object[1].pos.x = x;
	(*cur_obj).object[1].pos.y = y;

	map[(*cur_obj).layer][x][y] = (*cur_obj).repr;
	map_color[(*cur_obj).layer][x][y] = (*cur_obj).color;
	resource.spice -= (*cur_obj).cost;
	resource.population += (*cur_obj).population;
}

