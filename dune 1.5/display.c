/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"

// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
const POSITION sys_pos = { MAP_HEIGHT+1, 0 };
const POSITION sta_pos = { 1, MAP_WIDTH + 1 };
const POSITION ord_pos = { MAP_HEIGHT + 1, MAP_WIDTH + 1 };



char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

int color_backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
int color_frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };


bool change_condition[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // 맵에변화가 있을때

/* ================= game data =================== */
extern char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH];
extern int map_color[N_C_LAYER][MAP_HEIGHT][MAP_WIDTH];

extern int spice_num[MAP_HEIGHT][MAP_WIDTH];
extern int sys_clock;
char test_arr[6][10] = {"test1", "test2", "test3", "test4", "test5", "test6"};
struct {
	int cur;
	int size;
	char* arr[SYS_HEIGHT - 3];
	
}sys_log = {-1};

/* ================= state message =================== */
STATE_MESSAGE desert_s = {
	.size = 3,
	.about_size = 3,
	.message = {
		"[사막 지형]",
		"물 한 방울도 찾아볼 수 없다.",
		"모래 속에는 무엇인가 살고 있는 것 같다..."
	}
},
base_s = {
	.size = 2,
	.about_size = 3,
	.message = {
		"[본진]",
		"하베스터를 생산 할 수 있다."
	}

},
plate_s = {
	.size = 2,
	.about_size = 2,
	.message = {
		"[장판]",
		"위에 건물을 건설 할 수 있다."
	}
},
rock_s = {
	.size = 2,
	.about_size = 2,
	.message = {
		"[바위]",
		"평범한 바위이다."
	}
},
havester_s = {
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
spice_s = {
	.size = 4,
	.about_size = 3,
	.message = {
		"[스파이스]",
		"온통 모래뿐인 사막에서의 귀중한 자원이다.",
		"하베스터를 통해 얻을 수 있다.",
		"매장량 : "
	}
};

ORDER_LIST base_o = {
	.size = 1,
	.message = {
		"H : 하베스트 생산"
	}
};




void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);

char what_ch(POSITION pos) {
	switch (backbuf[pos.row][pos.column]) {
	case 'S': return '0' + spice_num[pos.row][pos.column];
	default:
		return backbuf[pos.row][pos.column];
	}

}
int what_color(POSITION pos) {
	return color_backbuf[pos.row][pos.column];
}

// 로그출력
// 출력할 문자열은 배열에 담아둠. -> 필요할때 꺼내쓸거임
// 로그 배열 -> 출력되어있는 문자열의 주소를 담음.
// print_message(위치, 문자열)

// 공용
void print_message(POSITION pos, char str[]) {
	gotoxy(pos);
	set_color(COLOR_DEFAULT);
	printf("%s", str);
}

void erase_message(POSITION pos, int size) {
	gotoxy(pos); 
	set_color(COLOR_DEFAULT); 
	for (int i = 0; i < size; i++) {
		printf(" ");
	}
}

void mesaage_animatinon(POSITION pos, char str[]) {
	gotoxy(pos); 
	set_color(COLOR_DEFAULT); 
	int idx = 0;
	while (str[idx] != '\0') {
		printf("%c", str[idx++]);
		Sleep(5);
	}
}

// 로그 배열의 문자열 교체
void insert_system_str(char new_str[]) {
	sys_log.cur = (sys_log.cur == SYS_HEIGHT - 3) ? 0 : sys_log.cur + 1; 

	sys_log.arr[sys_log.cur] = new_str;

	if (sys_log.size < SYS_HEIGHT - 3) sys_log.size++;
}

// 시스템 출력
void display_system_message(char new_str[]) {
	insert_system_str(new_str);
	POSITION pos = { SYS_HEIGHT - 3, 2 };
	int cnt = 1;
	int cur = (sys_log.cur == 0) ? SYS_HEIGHT - 4 : sys_log.cur - 1;
	while (cnt < sys_log.size) {
		erase_message(padd(sys_pos, pos), SYS_WIDTH-3); 
		print_message(padd(sys_pos, pos), sys_log.arr[cur]);
		pos.row -= 1;
		cnt++;
		cur = (cur == 0) ? SYS_HEIGHT-4 : cur - 1; 
	}
	pos.row = SYS_HEIGHT - 2;
	erase_message(padd(sys_pos, pos), SYS_WIDTH - 3);  
	mesaage_animatinon(padd(sys_pos, pos), sys_log.arr[sys_log.cur]);
}


// 상태창 출력
void print_state(char str[10][100], int about_size, int size) { 
	POSITION pos = { 3,2 };
	for (int i = 0; i < about_size; i++) {
		print_message(padd(sta_pos, pos), str[i]);  
		pos.row += 2; 
	}
	for (int i = about_size; i < size; i++) {
		print_message(padd(sta_pos, pos), str[i]);  
		pos.row += 1;
	}

}

void dispaly_object_info(CURSOR cursor) {
	POSITION pos = { 2,1 };
	for (int i = 0; i < STA_HEIGHT - 3; i++) {
		erase_message(padd(sta_pos, pos), STA_WIDTH - 2);
		pos.row += 1;
	}

	int curX = cursor.row;
	int curY = cursor.column;

	switch (backbuf[curX][curY]) {
	case 'B':
		print_state(base_s.message, base_s.about_size, base_s.size);
		break;
	case 'P':
		print_state(plate_s.message, plate_s.about_size, plate_s.size);
		break;
	case 'R':
		print_state(rock_s.message, rock_s.about_size, rock_s.size);
		break;
	case 'H':
		print_state(havester_s.message, havester_s.about_size, havester_s.size);
		break;
	case 'S':
		print_state(spice_s.message, spice_s.about_size, spice_s.size);
		printf("%d", spice_num[curX][curY]);
		break;
	default:
		print_state(desert_s.message, desert_s.about_size, desert_s.size);
		break;
	}
}

// 명령창 출력
void print_command(char str[3][100], int size) {
	POSITION pos = { 3,2 };
	for (int i = 0; i < size; i++) {
		print_message(padd(ord_pos, pos), str[i]); 
		pos.row += 1;
	}
}
void display_command(CURSOR cursor) { 
	POSITION pos = { 2,1 };
	for (int i = 0; i < CMD_HEIGHT - 3; i++) { 
		erase_message(padd(ord_pos, pos), CMD_WIDTH - 2); 
		pos.row += 1;
	}

	int curX = cursor.row;
	int curY = cursor.column;

	switch (backbuf[curX][curY]) {
	case 'B':
		print_command(base_o.message, base_o.size);
		break;

	}
}

// esc
void esc() {
	POSITION pos = { 2, 2 };
	for (int i = 0; i < STA_HEIGHT - 3; i++) {
		erase_message(padd(sta_pos, pos), STA_WIDTH - 3);
		pos.row += 1;
	}
	pos.row = 2;
	for (int i = 0; i < CMD_HEIGHT - 3; i++) { 
		erase_message(padd(ord_pos, pos), CMD_WIDTH - 3); 
		pos.row += 1; 
	}
}

void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], 
	CURSOR cursor)
{
	display_resource(resource);
	display_map(map);
	// 커서가 보이는게 거슬려서 커서가 이동할때 한번만 출력되게 변경. 그런데 이러면 다른 유닛이 커서를 지나가면 커서가 지워진다는 문제가 있음.
	// 커서를 최상위 레이어에 넣으면 해결. -> 레이어를 하나더 늘려야함.
	// map_color[3][][]에 커서를 넣어놓고 project 해서 출력. -> 현재 커서의 위치만 알고있으면됨
	// 커서의 이전위치, 현재위치를 저장하던 CURSOR 구조체를 POSTION 구조체로 통합.
	// display_cursor(cursor); 
}

void display_resource(RESOURCE resource) {
	set_color(COLOR_RESOURCE);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

// subfunction of draw_map()
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) { // map을 backduf에 복사
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}
void project_color(int src[N_C_LAYER][MAP_HEIGHT][MAP_WIDTH], int dest[MAP_HEIGHT][MAP_WIDTH]) { // map을 backduf에 복사
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int k = 0; k < N_C_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}


void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);
	project_color(map_color, color_backbuf);   

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j] || color_backbuf[i][j] != color_frontbuf[i][j]) {
				POSITION pos = {i, j};

				char ch = what_ch(pos); 
				int color = what_color(pos); 
				
				printc(padd(map_pos, pos), ch, color); 
			}
			frontbuf[i][j] = backbuf[i][j];
			color_frontbuf[i][j] = color_backbuf[i][j];
		}
	}
}


// 프레임 출력
void dispaly_frame() {
	display_system_frame();
	display_state_frame();
	display_command_frame();
}

void display_system_frame() {
	for (int j = 1; j < SYS_WIDTH - 1; j++) {
		POSITION pos = { 0, j };
		printc(padd(sys_pos, pos), '-', COLOR_DEFAULT);
		pos.row = SYS_HEIGHT - 1;
		printc(padd(sys_pos, pos), '-', COLOR_DEFAULT);
	}
	for (int i = 1; i < SYS_HEIGHT - 1; i++) {
		POSITION pos = { i, 0 };
		printc(padd(sys_pos, pos), '|', COLOR_DEFAULT);
		pos.column = SYS_WIDTH - 1;
		printc(padd(sys_pos, pos), '|', COLOR_DEFAULT);

	}

	POSITION pos = { 1, 2 };
	gotoxy(padd(sys_pos, pos));
	set_color(14);
	printf("system message");
}

void display_state_frame() {
	for (int j = 1; j < STA_WIDTH - 1; j++) {
		POSITION pos = { 0, j };
		printc(padd(sta_pos, pos), '-', COLOR_DEFAULT);
		pos.row = STA_HEIGHT - 1;
		printc(padd(sta_pos, pos), '-', COLOR_DEFAULT);
	}

	for (int i = 1; i < STA_HEIGHT - 1; i++) {
		POSITION pos = { i, 0 };
		printc(padd(sta_pos, pos), '|', COLOR_DEFAULT);
		pos.column = STA_WIDTH - 1;
		printc(padd(sta_pos, pos), '|', COLOR_DEFAULT);
	}

	POSITION pos = { 1, 2 };
	gotoxy(padd(sta_pos, pos));
	set_color(14);
	printf("state");
}

void display_command_frame() {
	for (int j = 1; j < CMD_WIDTH - 1; j++) {
		POSITION pos = { 0, j };
		printc(padd(ord_pos, pos), '-', COLOR_DEFAULT);
		pos.row = CMD_HEIGHT - 1; 
		printc(padd(ord_pos, pos), '-', COLOR_DEFAULT);
	}

	for (int i = 1; i < CMD_HEIGHT - 1; i++) { 
		POSITION pos = { i, 0 };
		printc(padd(ord_pos, pos), '|', COLOR_DEFAULT);
		pos.column = CMD_WIDTH - 1;
		printc(padd(ord_pos, pos), '|', COLOR_DEFAULT);
	}
	POSITION pos = { 1, 2 };
	gotoxy(padd(ord_pos, pos));
	set_color(14);
	printf("commands");
}

// 재출력
void re_display(RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor) {
	set_color(COLOR_DEFAULT);
	system("cls");
	dispaly_frame();
	display_resource(resource);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			POSITION pos = { i, j };
			char ch = what_ch(pos);
			int color = what_color(pos); 
			printc(padd(map_pos, pos), ch, color);

		}
	}
}



// 미완
/*
void BFS(CURSOR cursor) {
	int x = cursor.current.row;
	int y = cursor.current.column;
	char ch = map[0][x][y]; 
	if (ch == ' ') {
		display_system_message("구조물이 없습니다.");
		return;
	}

	POSITION Q[MAP_WIDTH * MAP_HEIGHT];
	bool vist[MAP_WIDTH][MAP_HEIGHT] = { 0 };
	int head = 0, tail = 0;

	Q[tail++] = cursor.current;
	vist[cursor.current.row][cursor.previous.column] = 1;
	change_condition[x][y] = 1;
	map_color[x][y].previous = map_color[x][y].current;
	map_color[x][y].current = COLOR_GREEN;
	while (head != tail) {
		POSITION cur = Q[head++];

		for (int i = 1; i <= 4; i++) {
			DIRECTION dir = { i };
			POSITION new_pos = pmove(cur, dir);
			int nx = new_pos.row;
			int ny = new_pos.column;

			if (nx < 1 || nx > MAP_HEIGHT - 2 || ny < 1 || ny > MAP_WIDTH - 2) continue;
			if (vist[nx][ny] || map[0][nx][ny] != ch) continue;

			vist[nx][ny] = 1;
			Q[tail++] = new_pos;
			change_condition[nx][ny] = 1;
			map_color[nx][ny].previous = map_color[nx][ny].current;
			map_color[nx][ny].current = COLOR_GREEN;
		}
	}
}
*/