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
void sand_worm_move();
POSITION obj_next_position(int);

// void sample_obj_move(void);
// POSITION sample_obj_next_position();


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { 1,1 };


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };
int map_color[N_C_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };     // 지형, 건물의 색 저장, map과 마찬가지로 투영시켜서 출력

int spice_num[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // 스파이스의 매장량



RESOURCE resource = {
	.spice = 0,
	.spice_max = 5,
	.population = 5,
	.population_max = 100
};

OBJECT obj[OBJ_NUM] = {
	// 1번
	{
	.layer = 1,
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'W',
	.color = COLOR_DY,
	.speed = 200,
	.next_move_time = 200
	},
	// 2번
	{
	.layer = 2,
		.pos = {2,25},
		.dest = {2, MAP_WIDTH - 2},
		.repr = 'E',
		.color = COLOR_PURPLE,
		.speed = 100,
		.next_move_time = 100
	},
	{
		.layer = 2,
		.pos = {MAP_HEIGHT - 2, 10}, 
		.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2}, 
		.repr = 'E',
		.color = COLOR_PURPLE,
		.speed = 100,
		.next_move_time = 100
	}

};



int double_click_distance = 1;
int vist_timer = -1;
KEY prev_key; 

/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();

	display(resource, map, cursor);
	dispaly_frame();

	while (1) {
		// loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인
		KEY key = get_key();

		if (vist_timer != -1 && vist_timer > 80) {
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
				display_system_message("1번");
				break;
			case k_2:
				display_system_message("2번");
				break;
			case k_space:
				display_command(cursor);  
				dispaly_object_info(cursor);
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
		// sample_obj_move();
		sand_worm_move();

		// 화면 출력
		display(resource, map, cursor);
		
		Sleep(TICK);
		sys_clock += 10;
		if (vist_timer != -1) vist_timer += 10;
	}
}

/* ================= subfunctions =================== */
void intro(void) {
	POSITION pos = {10, 53};
	gotoxy(pos);  
	printf("DUNE 1.5\n");

	pos.row += 2;
	pos.column = 27;
	gotoxy(pos);

	printf("화면이 깨진다면 맵 크기를 줄이거나 창을 전체화면으로 설정해 주세요.\n");

	pos.row += 2;
	pos.column = 40;
	gotoxy(pos);
	printf("r을 입력하면 화면이 다시 출력됩니다.\n");

	pos.row += 5;
	pos.column = 38;
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
	map[0][pos.row][pos.column] = 'R';
	map_color[0][pos.row][pos.column] = COLOR_GARY;
	map_color[0][pos.row][pos.column] = COLOR_GARY;
}

void init_4rock(POSITION pos) {
	for (int i = pos.row; i < pos.row + 2; i++) {
		for (int j = pos.column; j < pos.column + 2; j++) {
			POSITION pos = { i, j };
			init_rock(pos);
		}
	}
}

void init_obj() { 
	for (int i = 0; i < OBJ_NUM; i++) {
		int L = obj[i].layer;
		int x = obj[i].pos.row;
		int y = obj[i].pos.column;
		map[L][x][y] = obj[i].repr;
		map_color[L][x][y] = obj[i].color;
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
	POSITION pos1 = {MAP_HEIGHT-3, MAP_WIDTH-3};
	init_4rock(pos1);



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
	// 샌드웜, 독수리
	init_obj();

}


// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION dir, int n) { // 방향, 움직일 칸수
	if (n == 0) {
		vist_timer = -1;
		return;
	}

	POSITION curr = cursor; 
	POSITION new_pos = pmove(curr, dir); 
	// validation check
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 && \
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {
		map_color[3][cursor.row][cursor.column] = -1;
		cursor = new_pos;
		map_color[3][cursor.row][cursor.column] = COLOR_CURSOR; 
	}
	cursor_move(dir, n - 1);  
}


/* ================= sample object movement =================== */
POSITION obj_next_position(int i) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(obj[i].dest, obj[i].pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {

		if (obj[i].repr == 'E') {
			int nx = (obj[i].dest.row == MAP_HEIGHT - 2) ? 1 : obj[i].dest.row + 1;
			POSITION new_pos = { nx, MAP_WIDTH - 1 - obj[i].dest.column }; 
			obj[i].dest = new_pos;
		}
		else {
			// 랜덤목적지
			int nx = rand() % (MAP_HEIGHT - 2) + 1;
			int ny = rand() % (MAP_WIDTH - 2) + 1;

			POSITION new_pos = { nx, ny };
			obj[i].dest = new_pos;
		}

		/*
		if (obj[idx].dest.row == 1 && obj[idx].dest.column == 1) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj[idx].dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { 1, 1 };
			obj[idx].dest = new_dest;
		}
		*/

		dir = d_stay; 
	}	
	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	else if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	POSITION next_pos = pmove(obj[i].pos, dir); 
	if (next_pos.row < 1 || next_pos.row > MAP_HEIGHT - 2 || \
		 next_pos.column < 1 || next_pos.column > MAP_WIDTH - 2) { // 맵을 벗어날때

		return obj[i].pos;
	}
	else if (map[1][next_pos.row][next_pos.column] > 0) { // 가려는 위치에 다른 유닛이 있을때
		// return sand_worm[idx].pos;
		return next_pos;
	}
	else {

		return next_pos;  // 다음 위치
	}
	
}

void sand_worm_move(void) { 
	for (int i = 0; i < OBJ_NUM; i++) {
		if (sys_clock <= obj[i].next_move_time) continue;

		int r = obj[i].pos.row;
		int c = obj[i].pos.column;

		// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
		map[obj[i].layer][r][c] = -1; // 유닛 지나간 자리 복구
		map_color[obj[i].layer][r][c] = -1;

		// 움직일 자리
		obj[i].pos = obj_next_position(i);
		r = obj[i].pos.row;
		c = obj[i].pos.column;

		map[obj[i].layer][r][c] = obj[i].repr;
		map_color[obj[i].layer][r][c] = obj[i].color; // 배경색상 변경 

		obj[i].next_move_time = sys_clock + obj[i].speed;
	}
}