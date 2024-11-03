/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include "display.h"
#include "common.h"
#include "io.h"

// ����� ������� �»��(topleft) ��ǥ - > ������ ����
const POSITION resource_pos = { 0, 0 };
const POSITION time_pos = { 0, MAP_WIDTH - 13 };
const POSITION map_pos = { 2, 1 };
const POSITION sys_pos = { MAP_HEIGHT + 4,  2 };
const POSITION state_pos = { 2,  MAP_WIDTH + 5 };
const POSITION cmd_pos = { MAP_HEIGHT + 4,  MAP_WIDTH + 5 };
extern int sys_clock;

// �ý��� ���ڿ�
struct {
	int cur;
	int size;
	char arr[SYS_HEIGHT - 1][SYS_WIDTH * 2];
}sys_log = { -1 };


extern UNIT unit_map[MAP_HEIGHT][MAP_WIDTH];
extern BUILDING building_map[MAP_HEIGHT][MAP_WIDTH];
extern NATURE_INFO* nature_map[MAP_HEIGHT][MAP_WIDTH];
extern SKY sky_map[MAP_HEIGHT][MAP_WIDTH];
extern SKY eagle;
extern bool map_change[MAP_HEIGHT][MAP_WIDTH];

/* ================= function =================== */


void display_resource(RESOURCE resource) {
	set_color(COLOR_RESOURCE);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}
void display_time() {
	set_color(COLOR_RESOURCE);
	gotoxy(time_pos);
	printf("time: %d: %d: %d", sys_clock / 60000, (sys_clock / 1000) % 60, (sys_clock / 10) % 60);
}
void display_map(POSITION cursor) {
	POSITION pos = { 2, 1 };
	for (int r = 0; r < MAP_HEIGHT; r++) {
		for (int c = 0; c < MAP_WIDTH; c++) {
			if (map_change[r][c])
			{
				if (eagle.pos.x == r && eagle.pos.y == c) {
					int color = (cursor.x == r && cursor.y == c) ? COLOR_GREEN : eagle.color;
					printc(padd(pos, (POSITION) { r, c }), eagle.repr, color);
				}
				else if (unit_map[r][c].exist) {
					int color = (cursor.x == r && cursor.y == c) ? COLOR_GREEN : unit_map[r][c].info_p->color;
					printc(padd(pos, (POSITION) { r, c }), unit_map[r][c].info_p->repr, color);
				}
				else if (building_map[r][c].exist) {
					int color = (cursor.x == r && cursor.y == c) ? COLOR_GREEN : building_map[r][c].info_p->color;
					if (building_map[r][c].info_p->repr == 'S')
						printc(padd(pos, (POSITION) { r, c }), '0' + building_map[r][c].hp, color);
					else
						printc(padd(pos, (POSITION) { r, c }), building_map[r][c].info_p->repr, color);
				}
				else {
					int color = (cursor.x == r && cursor.y == c) ? COLOR_GREEN : nature_map[r][c]->color;
					printc(padd(pos, (POSITION) { r, c }), nature_map[r][c]->repr, color);
				}
			}
			map_change[r][c] = 0;
		}
	}
}

// ������ ���
void display_map_frame() {
	for (int c = 0; c <= MAP_WIDTH + 1; c++) {
		printc((POSITION) { 1, c }, '#', COLOR_DEFAULT);
		printc((POSITION) { MAP_HEIGHT + 2, c }, '#', COLOR_DEFAULT);
	}
	for (int r = 2; r <= MAP_HEIGHT + 1; r++) {
		printc((POSITION) { r, 0 }, '#', COLOR_DEFAULT);
		printc((POSITION) { r, MAP_WIDTH + 1 }, '#', COLOR_DEFAULT);
	}
}
void display_system_frame() {
	int pos_r = MAP_HEIGHT + 3;
	for (int c = 1; c <= SYS_WIDTH; c++) {
		printc((POSITION) { pos_r, c }, '-', COLOR_DEFAULT);
		printc((POSITION) { pos_r + SYS_HEIGHT + 1, c }, '-', COLOR_DEFAULT);
	}
	for (int r = 1; r <= SYS_HEIGHT; r++) {
		printc((POSITION) { pos_r + r, 0 }, '|', COLOR_DEFAULT);
		printc((POSITION) { pos_r + r, SYS_WIDTH + 1 }, '|', COLOR_DEFAULT);
	}


	gotoxy(sys_pos);
	set_color(14);
	printf("system message");
}
void display_state_frame() {
	int pos_c = MAP_WIDTH + 3;
	for (int c = 1; c <= STA_WIDTH; c++) {
		printc((POSITION) { 1, pos_c + c }, '-', COLOR_DEFAULT);
		printc((POSITION) { MAP_HEIGHT + 2, pos_c + c }, '-', COLOR_DEFAULT);
	}
	for (int r = 2; r <= STA_HEIGHT + 1; r++) {
		printc((POSITION) { r, pos_c }, '|', COLOR_DEFAULT);
		printc((POSITION) { r, pos_c + STA_WIDTH + 1 }, '|', COLOR_DEFAULT);
	}

	gotoxy(state_pos);
	set_color(14);
	printf("state");
}
void display_command_frame() {
	int pos_r = MAP_HEIGHT + 3;
	int pos_c = MAP_WIDTH + 3;
	for (int c = 1; c <= CMD_WIDTH; c++) {
		printc((POSITION) { pos_r, pos_c + c }, '-', COLOR_DEFAULT);
		printc((POSITION) { pos_r + CMD_HEIGHT + 1, pos_c + c }, '-', COLOR_DEFAULT);
	}

	for (int r = 1; r <= CMD_HEIGHT; r++) {
		printc((POSITION) { pos_r + r, pos_c }, '|', COLOR_DEFAULT);
		printc((POSITION) { pos_r + r, pos_c + STA_WIDTH + 1 }, '|', COLOR_DEFAULT);
	}


	gotoxy(cmd_pos);
	set_color(14);
	printf("command");
}
void display_frame() {
	display_system_frame();
	display_state_frame();
	display_command_frame();
	display_map_frame();
}
void re_display(RESOURCE resource, POSITION cursor,
	bool chage_map[MAP_HEIGHT][MAP_WIDTH]) {
	set_color(COLOR_DEFAULT);
	system("cls");
	display_frame();
	display_resource(resource);

	for (int r = 0; r < MAP_HEIGHT; r++) {
		for (int c = 0; c < MAP_WIDTH; c++) {
			chage_map[r][c] = 1;
		}
	}
	display_map(cursor);
}


// ���ڿ� ����Լ�
void print_message(POSITION pos, char str[]) {
	gotoxy(pos);
	set_color(COLOR_DEFAULT);
	printf("%s", str);
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
void erase_message(POSITION pos, int size) {
	gotoxy(pos);
	set_color(COLOR_DEFAULT);
	for (int i = 0; i < size; i++) {
		printf(" ");
	}
}

// �ý���â
// �ý��� �迭�� ���ڿ� ��ü
void insert_system_str(char new_str[]) {
	sys_log.cur = (sys_log.cur == SYS_HEIGHT - 2) ? 0 : sys_log.cur + 1;

	snprintf(sys_log.arr[sys_log.cur], SYS_WIDTH * 2, new_str);

	if (sys_log.size < SYS_HEIGHT - 1) sys_log.size++;
}
// �ý��� ���
void display_system_message(char new_str[]) {
	insert_system_str(new_str);
	POSITION pos = { SYS_HEIGHT - 2, 0 };
	int cnt = 1;
	int cur = (sys_log.cur == 0) ? SYS_HEIGHT - 2 : sys_log.cur - 1;
	while (cnt < sys_log.size) {
		erase_message(padd(sys_pos, pos), SYS_WIDTH - 2);
		print_message(padd(sys_pos, pos), sys_log.arr[cur]);
		pos.x -= 1;
		cnt++;
		cur = (cur == 0) ? SYS_HEIGHT - 2 : cur - 1;
	}
	pos.x = SYS_HEIGHT - 1;
	erase_message(padd(sys_pos, pos), SYS_WIDTH - 2);
	mesaage_animatinon(padd(sys_pos, pos), sys_log.arr[sys_log.cur]);
}

// ����â
STATE_MESSAGE* get_state_message(POSITION pos) {
	if (eagle.pos.x == pos.x && eagle.pos.y == pos.y) {
		return &eagle.state_message;
	}
	else if (unit_map[pos.x][pos.y].exist) {
		return &unit_map[pos.x][pos.y].info_p->state_message;
	}
	else if (building_map[pos.x][pos.y].exist) {
		return &building_map[pos.x][pos.y].info_p->state_message;
	}
	else {
		return &nature_map[pos.x][pos.y]->state_message;
	}
}
void dispaly_state_message(POSITION selection_pos) {
	for (int r = 0; r < STA_HEIGHT - 2; r++) {
		erase_message(padd(state_pos, (POSITION) { 2 + r, 0 }), STA_WIDTH - 1);
	}

	STATE_MESSAGE* message = get_state_message(selection_pos);
	POSITION pos = { 2, 0 };
	for (int r = 0; r < message->about_size; r++) {
		print_message(padd(state_pos, pos), message->message[r]);
		pos.x += 2;
	}
	for (int i = message->about_size; i < message->size; i++) {
		print_message(padd(state_pos, pos), message->message[i]);
		pos.x += 1;
	}

	pos.x += 1;
	// �����̽��� ���, ���差�� ���
	if (building_map[selection_pos.x][selection_pos.y].exist && building_map[selection_pos.x][selection_pos.y].info_p->repr == 'S') {
		gotoxy(padd(state_pos, pos));
		printf("���差 : %d", building_map[selection_pos.x][selection_pos.y].hp);
	}
	else if (building_map[selection_pos.x][selection_pos.y].exist && building_map[selection_pos.x][selection_pos.y].hp > 0) {
		gotoxy(padd(state_pos, pos));
		printf("����ü�� : ");
		for (int i = 0; i < building_map[selection_pos.x][selection_pos.y].hp / 10; i++) printf("@");
		printf(" (%d)", building_map[selection_pos.x][selection_pos.y].hp);
	}
	else if (unit_map[selection_pos.x][selection_pos.y].exist) {
		gotoxy(padd(state_pos, pos));
		printf("����ü�� : ");
		if (unit_map[selection_pos.x][selection_pos.y].info_p->repr == 'W') {
			printf("�� �� ����");
		}
		else {
			for (int i = 0; i < unit_map[selection_pos.x][selection_pos.y].hp / 10; i++) printf("@");
			printf(" (%d)", unit_map[selection_pos.x][selection_pos.y].hp);
		}
	}

}

// ���â
CMD_MESSAGE* get_cmd_message(POSITION pos) {
	if (unit_map[pos.x][pos.y].exist) {
		return &unit_map[pos.x][pos.y].info_p->cmd_message;
	}
	else if (building_map[pos.x][pos.y].exist) {
		return &building_map[pos.x][pos.y].info_p->cmd_message;
	}
	else {
		return 0;
	}
}
void display_cmd_message(POSITION selection_pos) {
	for (int r = 0; r < CMD_HEIGHT - 2; r++) {
		erase_message(padd(cmd_pos, (POSITION) { 2 + r, 0 }), STA_WIDTH - 1);
	}

	CMD_MESSAGE* message = get_cmd_message(selection_pos);
	if (message) {
		POSITION pos = { 2, 0 };
		for (int i = 0; i < message->size; i++) {
			print_message(padd(cmd_pos, pos), message->message[i]);
			pos.x += 2;
		}
	}

}

// ���� ���
void esc(SELECTION* selection) {
	selection->pos = (POSITION){ 0, 0 };
	selection->repr = ' ';

	for (int r = 0; r < STA_HEIGHT - 2; r++) {
		erase_message(padd(state_pos, (POSITION) { 2 + r, 0 }), STA_WIDTH - 1);
	}
	for (int r = 0; r < CMD_HEIGHT - 2; r++) {
		erase_message(padd(cmd_pos, (POSITION) { 2 + r, 0 }), STA_WIDTH - 1);
	}
}