/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include "display.h"
#include "io.h"

// ����� ������� �»��(topleft) ��ǥ
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
const POSITION sys_pos = { MAP_HEIGHT+1, 0 };
const POSITION sta_pos = { 1, MAP_WIDTH + 1 };
const POSITION ord_pos = { MAP_HEIGHT + 1, MAP_WIDTH + 1 };



char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

/* ================= game data =================== */
extern map[N_LAYER][MAP_HEIGHT][MAP_WIDTH];
extern COLOR map_color[MAP_HEIGHT][MAP_WIDTH]; 
QUEUE sys_message = {
	.log_q = {0},
	.head = 0,
	.tail = 0
};


void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);

char what_ch(POSITION pos) {
	return backbuf[pos.row][pos.column];
}
int what_color(POSITION pos) {
	switch (backbuf[pos.row][pos.column]) {
	case '#': return COLOR_DEFAULT;
	case ' ': return COLOR_YELLOW;  
	default: return  map_color[pos.row][pos.column].current;
	}
}
void erase_message(POSITION pos, int size) {
	gotoxy(padd(sys_pos, pos));
	set_color(COLOR_DEFAULT);
	for (int i = 0; i < size - 3; i++) {
		printf(" ");
	}
}
void display_message(char str[], int size) {
	POSITION pos = { SYS_HEIGHT-2, 2 }; 
	erase_message(pos, SYS_WIDTH);
	gotoxy(padd(sys_pos, pos));
	set_color(COLOR_DEFAULT);

	for (int i = 0; i < size; i++) {
		printf("%c", str[i]);
		Sleep(50);
	}
}
void scroll_message() {

}


void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], 
	CURSOR cursor)
{
	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
	// display_system_message();  
	// display_object_info()
	// display_commands()
	// ...
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
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) { // map�� backduf�� ����
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

// map�迭�� ��ȭ�� �������� ����ϱ� ������ �������� ��ȭ�� ������ map�迭�� �ٲ������
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				POSITION pos = {i, j};
				char ch = what_ch(pos); 
				int color = what_color(pos); 
				
				printc(padd(map_pos, pos), ch, color); 
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}

// Ŀ�� ��ġ�� ���ڸ� ���� �ٲ㼭 �״�� �ٽ� ���
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	// ������ġ �� ����
	char ch = what_ch(prev);
	int color = what_color(prev); 
	printc(padd(map_pos, prev), ch, color);

	// Ŀ�� ��ġ �� ����
	ch = what_ch(curr);
	printc(padd(map_pos, curr), ch, COLOR_GREEN);
}



// ������ ���

void dispaly_frame() {
	display_system_frame();
	display_state_frame();
	display_order_frame();
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

void display_order_frame() {
	for (int j = 1; j < ORD_WIDTH - 1; j++) {
		POSITION pos = { 0, j };
		printc(padd(ord_pos, pos), '-', COLOR_DEFAULT);
		pos.row = ORD_HEIGHT - 1;
		printc(padd(ord_pos, pos), '-', COLOR_DEFAULT);
	}

	for (int i = 1; i < ORD_HEIGHT - 1; i++) {
		POSITION pos = { i, 0 };
		printc(padd(ord_pos, pos), '|', COLOR_DEFAULT);
		pos.column = ORD_WIDTH - 1;
		printc(padd(ord_pos, pos), '|', COLOR_DEFAULT);
	}
	POSITION pos = { 1, 2 };
	gotoxy(padd(ord_pos, pos));
	set_color(14);
	printf("order message");
}

// �����
void re_display(RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor) {
	set_color(COLOR_DEFAULT);
	system("cls");
	dispaly_frame();
	display_resource(resource);
	display_cursor(cursor);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			POSITION pos = { i, j };
			char ch = what_ch(pos);
			int color = what_color(pos); 
			printc(padd(map_pos, pos), ch, color);

		}
	}

}