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
POSITION sand_worm_next_position(int);

// void sample_obj_move(void);
// POSITION sample_obj_next_position();


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };
COLOR map_color[MAP_HEIGHT][MAP_WIDTH] = { 0 };     // 지형, 건물의 색 저장 




RESOURCE resource = {
	.spice = 0,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
};

SAND_WORM obj = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'W',
	.speed = 100,
	.next_move_time = 300
};

SAND_WORM sand_worm[SAND_WORM_NUM] = { 
	// 1번
	{
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'W',
	.speed = 100,
	.next_move_time = 100
	},
	// 2번
	{
	.pos = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.dest = {1,1},
	.repr = 'W',
	.speed = 200,
	.next_move_time = 200
	}

};


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

		if (vist_timer != -1 && vist_timer > 80) cursor_move(ktod(prev_key), 1);   
		// 키 입력이 있으면 처리
		if (is_arrow_key(key)) {
			if (vist_timer == -1) {
				vist_timer = 0;
				prev_key = key;
			}
			else {
				if (prev_key == key)
					cursor_move(ktod(key), 5); 
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
				display_system_message("new str");
				break;
			case k_1:
				display_system_message("1번");
				break;
			case k_2:
				display_system_message("2번");
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
	POSITION pos = {10, 50};
	gotoxy(pos);  
	printf("DUNE 1.5\n");

	pos.row += 2;
	pos.column = 27;
	gotoxy(pos);

	printf("창을 전체화면으로 설정해 주세요. (창모드 시 맵 크기가 크면 깨져요 ㅠㅠ)\n");

	pos.row += 2;
	pos.column = 40;
	gotoxy(pos);
	printf("화면이 깨질때는 r을 입력해주세요.\n");

	pos.row += 5;
	pos.column = 40;
	gotoxy(pos);

	printf("계속하려면 아무키나 입력해주세요\n");
	_getch();
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
	map_color[pos.row][pos.column].current = COLOR_GARY;
	map_color[pos.row][pos.column].previous = COLOR_GARY;

}

void init_4rock(POSITION pos) {
	for (int i = pos.row; i < pos.row + 2; i++) {
		for (int j = pos.column; j < pos.column + 2; j++) {
			POSITION pos = { i, j };
			init_rock(pos);
		}
	}
}

void init(void) {
	// layer 0(map[0])에 지형 생성

	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';
		}
	}

	// 블루팀
	for (int i = MAP_HEIGHT - 3; i < MAP_HEIGHT - 1; i++) {
		for (int j = 1; j < 3; j++) {
			map[0][i][j] = 'B';
			map_color[i][j].current = COLOR_BULE; 
			map_color[i][j].previous = COLOR_BULE;
		}
		for (int j = 3; j < 5; j++) {
			map[0][i][j] = 'P';
			map_color[i][j].current = COLOR_BLACk;
			map_color[i][j].previous = COLOR_BLACk;
		}
	}
	
	// 레드팀
	for (int i = 1; i < 3; i++) {
		for (int j = MAP_WIDTH - 3; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = 'B';
			map_color[i][j].current = COLOR_RED;
			map_color[i][j].previous = COLOR_RED;
		}
		for (int j = MAP_WIDTH - 5; j < MAP_WIDTH - 3; j++) {
			map[0][i][j] = 'P';
			map_color[i][j].current = COLOR_BLACk;
			map_color[i][j].previous = COLOR_BLACk;
		}
	}

	// 스파이스는 이름 대신 매장량을 map배열에 저장
	map[0][MAP_HEIGHT - 6][1] = '0' + HAV_NUM; // 블루팀 
	map_color[MAP_HEIGHT - 6][1].current = COLOR_DY; 
	map_color[MAP_HEIGHT - 6][1].previous = COLOR_DY;

	map[0][5][MAP_WIDTH - 2] = '0' + HAV_NUM;  // 레드팀 
	map_color[5][MAP_WIDTH - 2].current = COLOR_DY; 
	map_color[5][MAP_WIDTH - 2].previous = COLOR_DY;

	

	// 돌
	POSITION pos[5] = { {5,16}, {MAP_HEIGHT - 7, MAP_WIDTH - 18},  { MAP_HEIGHT - 7,10 }, {6, MAP_WIDTH - 11}, {MAP_HEIGHT/2, MAP_WIDTH/2}}; 
	init_4rock(pos[0]);
	init_4rock(pos[1]);
	init_rock(pos[2]);
	init_rock(pos[3]);
	init_rock(pos[4]);
	POSITION pos1 = {MAP_HEIGHT-3, MAP_WIDTH-3};
	init_4rock(pos1);


	// layer 1(map[1])은 비워 두기(-1로 채움)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	// 하베스터
	map[1][MAP_HEIGHT - 4][1] = 'H';
	map_color[MAP_HEIGHT - 4][1].current = COLOR_BULE; 

	map[1][3][MAP_WIDTH - 2] = 'H';
	map_color[3][MAP_WIDTH - 2].current = COLOR_RED;

	// object sample
	map[1][obj.pos.row][obj.pos.column] = obj.repr;
}


// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION dir, int n) { // 방향, 움직일 칸수
	if (n == 0) {
		vist_timer = -1;
		return;
	}

	POSITION curr = cursor.current; 
	POSITION new_pos = pmove(curr, dir); 
	// validation check
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 && \
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {
		cursor.current = new_pos;
	}
	cursor_move(dir, n - 1);  
	cursor.previous = curr; // cursor.previous를 함수에 처음 들어왔을때의 cursor.current값으로 맞춰줌
}


/* ================= sample object movement =================== */
POSITION sand_worm_next_position(int idx) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(sand_worm[idx].dest, sand_worm[idx].pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (sand_worm[idx].dest.row == 1 && sand_worm[idx].dest.column == 1) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			sand_worm[idx].dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { 1, 1 };
			sand_worm[idx].dest = new_dest;
		}
	}

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	
	POSITION next_pos = pmove(sand_worm[idx].pos, dir); 
	if (next_pos.row < 1 || next_pos.row > MAP_HEIGHT - 2 || \
		 next_pos.column < 1 || next_pos.column > MAP_WIDTH - 2) { // 맵을 벗어날때

		return sand_worm[idx].pos;
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
	for (int i = 0; i < SAND_WORM_NUM; i++) {
		if (sys_clock <= sand_worm[i].next_move_time) continue;

		int r = sand_worm[i].pos.row;
		int c = sand_worm[i].pos.column;

		// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
		map[1][r][c] = -1; // 샌드웜이 지나간 자리 복구
		map_color[r][c].current = map_color[r][c].previous; // 색상 원복  

		// 움직일 자리
		sand_worm[i].pos = sand_worm_next_position(i);
		r = sand_worm[i].pos.row;
		c = sand_worm[i].pos.column;

		map[1][r][c] = sand_worm[i].repr;
		map_color[r][c].previous = map_color[r][c].current;
		map_color[r][c].current = COLOR_DY; // 배경색상 변경 

		sand_worm[i].next_move_time = sys_clock + sand_worm[i].speed;
	}
}

/*
POSITION sample_obj_next_position(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj.pos;  // 제자리
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// 아직 시간이 안 됐음
		return;
	}

	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
	map[1][obj.pos.row][obj.pos.column] = -1;
	map_color[obj.pos.row][obj.pos.column].current = map_color[obj.pos.row][obj.pos.column].previous; // 색상 원복

	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;
	map_color[obj.pos.row][obj.pos.column].previous = map_color[obj.pos.row][obj.pos.column].current;
	map_color[obj.pos.row][obj.pos.column].current = COLOR_DY; // 배경색상 변경


	obj.next_move_time = sys_clock + obj.speed;
}
*/