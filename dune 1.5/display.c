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

// �ý��� ���ڿ�
struct {
	int cur;
	int size;
	char arr[SYS_HEIGHT - 1][SYS_WIDTH * 2];
}sys_log = { -1 };

/* engine data */
extern NATURE_INFO* map[MAP_HEIGHT][MAP_WIDTH];
extern UNIT units[100];
extern BUILDING buildings[100];
extern SANDWORM sandworm[2];
extern bool map_change[MAP_HEIGHT][MAP_WIDTH];
extern POSITION cursor;
extern RESOURCE resource;
extern STORM storm;
extern EAGLE eagle;
extern int sys_clock;


// ���Ḯ��Ʈ���� �ش��ϴ� ��ü�� �ε������� ��ȯ�ϴ� �Լ�
int get_sandworm_idx(POSITION pos) {
	for (int i = 0; i < 2; i++) {
		if (sandworm[i].pos.x == pos.x && sandworm[i].pos.y == pos.y) return i;
	}
	return 3;
}
int get_unit_idx(POSITION pos) { // �ش���ġ�� �ִ� ������ �ε����� ��ȯ
	int idx = 0;
	while (units[units[idx].next].exist) {
		idx = units[idx].next;
		if (units[idx].pos.x == pos.x && units[idx].pos.y == pos.y) {
			return idx;
		}
	}
	return 0;
}
int get_building_idx(POSITION pos) {
	int idx = 0;
	while (buildings[buildings[idx].next].exist) {
		idx = buildings[idx].next;
		for (int r = 0; r < buildings[idx].info_p->size; r++) {
			for (int c = 0; c < buildings[idx].info_p->size; c++) {
				POSITION n_pos = padd(buildings[idx].pos, (POSITION) { r, c });
				if (pos.x == n_pos.x && pos.y == n_pos.y) return idx;
			}
		}
	}
	return 0;
}
int get_storm_idx(POSITION pos) {
	int idx = 0;
	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			POSITION npos = padd(storm.pos, (POSITION) { r, c });
			if (npos.x == pos.x && npos.y == pos.y) {
				return idx;
			}
			idx++;
		}
	}
	return -1;
}

// ������ ����Լ�
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
void display_resource() {
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
void display() {
	display_resource();
	display_time();
	display_frame();

	// �ʱ� �� ���

	// nature
	for (int r = 0; r < MAP_HEIGHT; r++) {
		for (int c = 0; c < MAP_WIDTH; c++) {
			printc(padd(map_pos, (POSITION) { r, c }), map[r][c]->repr, map[r][c]->color);
		}
	}

	// building
	int idx = 0;
	while (buildings[buildings[idx].next].exist) {
		idx = buildings[idx].next;
		char repr = (buildings[idx].info_p->repr == 'S') ? '0' + buildings[idx].hp : buildings[idx].info_p->repr;
		for (int r = 0; r < buildings[idx].info_p->size; r++) {
			for (int c = 0; c < buildings[idx].info_p->size; c++) {
				POSITION pos = padd(buildings[idx].pos, (POSITION) { r, c });
				printc(padd(map_pos, pos), repr, buildings[idx].info_p->color);
			}
		}
	}

	// units
	idx = 0;
	while (units[units[idx].next].exist) {
		idx = units[idx].next;
		printc(padd(map_pos, (POSITION) { units[idx].pos.x, units[idx].pos.y }), units[idx].info_p->repr, units[idx].info_p->color);
	}

	// sandworm
	for (int i = 0; i < 2; i++) {
		int color = (i == 0) ? COLOR_CURSOR : sandworm[i].info_p->color;
		printc(padd(map_pos, (POSITION) { sandworm[i].pos.x, sandworm[i].pos.y }), sandworm[i].info_p->repr, color);
	}
	printc(padd(map_pos, (POSITION) { eagle.pos.x, eagle.pos.y }), eagle.repr, eagle.color);

}


// ����� �Լ�
void display_map() {
	for (int r = 0; r < MAP_HEIGHT; r++) {
		for (int c = 0; c < MAP_WIDTH; c++) {
			if (map_change[r][c]) {
				if (storm.exist) {
					int idx = get_storm_idx((POSITION) { r, c });
					if (idx != -1) {
						unsigned char color = (cursor.x == r && cursor.y == c) ? COLOR_CURSOR : storm.color;
						printc(padd(map_pos, (POSITION) { r, c }), storm.repr[idx], color);
						map_change[r][c] = 0;
						continue;
					}
				}
				if (eagle.pos.x == r && eagle.pos.y == c) {
					unsigned char color = (cursor.x == r && cursor.y == c) ? COLOR_CURSOR : eagle.color;
					printc(padd(map_pos, (POSITION) { r, c }), eagle.repr, color);
					map_change[r][c] = 0;
					continue;
				}
				int idx = get_unit_idx((POSITION) { r, c });
				if (idx) { // ������ �����Ұ��
					unsigned char color = (cursor.x == r && cursor.y == c) ? COLOR_CURSOR : units[idx].info_p->color;
					printc(padd(map_pos, (POSITION) { r, c }), units[idx].info_p->repr, color);
					map_change[r][c] = 0;
					continue;
				}
				idx = get_sandworm_idx((POSITION) { r, c });
				if (idx != 3) {
					unsigned char color = (cursor.x == r && cursor.y == c) ? COLOR_CURSOR : sandworm[idx].info_p->color;
					printc(padd(map_pos, (POSITION) { r, c }), sandworm[idx].info_p->repr, color);
					map_change[r][c] = 0;
					continue;
				}
				idx = get_building_idx((POSITION) { r, c });
				if (idx) { // �ǹ��� �����Ұ��
					unsigned char color = (cursor.x == r && cursor.y == c) ? COLOR_CURSOR : buildings[idx].info_p->color;
					char repr = (buildings[idx].info_p->repr == 'S') ? '0' + buildings[idx].hp : buildings[idx].info_p->repr;
					printc(padd(map_pos, (POSITION) { r, c }), repr, color);
					map_change[r][c] = 0;
					continue;
				}
				unsigned char color = (cursor.x == r && cursor.y == c) ? COLOR_CURSOR : map[r][c]->color;
				printc(padd(map_pos, (POSITION) { r, c }), map[r][c]->repr, color);
				map_change[r][c] = 0;
			}
		}
	}
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


// �ý���â ����Լ�
void insert_system_str(char new_str[]) { // �ý��� �迭�� ���ο� ���ڿ��� �����ϰ�, Ŀ��(�ؿ��� �ι�° �� ���ڿ�)�� �̵�
	sys_log.cur = (sys_log.cur == SYS_HEIGHT - 2) ? 0 : sys_log.cur + 1;

	snprintf(sys_log.arr[sys_log.cur], SYS_WIDTH * 2, new_str);

	if (sys_log.size < SYS_HEIGHT - 1) sys_log.size++;
}
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

// ����â ����Լ�
STATE_MESSAGE* get_state_message(POSITION pos) { // �ش���ġ ��ü�� state message�� ��ȯ
	if (pos.x == eagle.pos.x && pos.y == eagle.pos.y) {
		return &eagle.state_message;
	}
	int idx = get_unit_idx(pos);
	if (idx) {
		return &units[idx].info_p->state_message;
	}
	idx = get_sandworm_idx(pos);
	if (idx != 3) {
		return &sandworm[idx].info_p->state_message;
	}
	idx = get_building_idx(pos);
	if (idx) {
		return &buildings[idx].info_p->state_message;
	}
	return &map[pos.x][pos.y]->state_message;
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
	int idx = get_building_idx(selection_pos);
	if (idx && buildings[idx].info_p->repr == 'S') {
		gotoxy(padd(state_pos, pos));
		printf("���差 : %d", buildings[idx].hp);
		return;
	}
	else if (idx && buildings[idx].hp > 0) {
		gotoxy(padd(state_pos, pos));
		printf("����ü�� : ");
		for (int i = 0; i < buildings[idx].hp / 10; i++) printf("@");
		printf(" (%d)", buildings[idx].hp);
		return;
	}
	idx = get_unit_idx(selection_pos);
	if (idx) {
		gotoxy(padd(state_pos, pos));
		printf("����ü�� : ");

		for (int i = 0; i < units[idx].hp / 10; i++) printf("@");
		printf(" (%d)", units[idx].hp);

	}
	idx = get_sandworm_idx(selection_pos);
	if (idx != 3) {
		gotoxy(padd(state_pos, pos));
		printf("����ü�� : �� �� ����");
		pos.x += 2;
		char buff[100];
		snprintf(buff, 100, "���� ���� �ð� : %d : %d : %d",
			sandworm[idx].next_emission_time / 60000,
			(sandworm[idx].next_emission_time / 1000) % 60,
			(sandworm[idx].next_emission_time / 10) % 60);
		gotoxy(padd(state_pos, pos));
		printf(buff);
	}
}

// ���â ����Լ�
CMD_MESSAGE* get_cmd_message(POSITION pos) { //
	int idx = get_unit_idx(pos);
	if (idx) {
		return &units[idx].info_p->cmd_message;
	}
	idx = get_building_idx(pos);
	if (idx) {
		if (buildings[idx].info_p->color != COLOR_RED)
			return &buildings[idx].info_p->cmd_message;
	}
	return 0;
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

void re_display() {
	set_color(COLOR_DEFAULT);
	system("cls");
	display_frame();
	display_resource(resource);

	for (int r = 0; r < MAP_HEIGHT; r++) {
		for (int c = 0; c < MAP_WIDTH; c++) {
			map_change[r][c] = 1;
		}
	}
	display_map(cursor);
}