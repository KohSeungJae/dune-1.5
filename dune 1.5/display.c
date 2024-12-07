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
extern RESOURCE r_resource;
extern STORM storm;
extern EAGLE eagle;
extern int sys_clock;
extern bool build_mode;
extern UNIT* selected_unit;
extern BUILDING* selected_building;
extern char plate[MAP_HEIGHT][MAP_WIDTH];
extern BUILDING_INFO b_plate_info;
extern BUILDING_INFO r_plate_info;
extern char cmd_mode[20];


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
void display_resource(bool* resource_change) {
	if (!*resource_change) return;
	*resource_change = 0;
	set_color(COLOR_DEFAULT); 
	gotoxy(resource_pos);
	for (int i = 0; i < 33; i++) printf(" ");
	set_color(COLOR_RESOURCE); 
	gotoxy(resource_pos);
	printf("spice = %d/%d, population = %d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}
void display_time() {
	set_color(COLOR_RESOURCE);
	gotoxy(time_pos);
	printf("time: %d: %d: %d", sys_clock / 60000, (sys_clock / 1000) % 60, (sys_clock / 10) % 60);
}

// Ŀ�� ���
void display_cursor() {
	int rx = 1, cx = 1;
	if (build_mode) {
		rx = 2;
		cx = 2;
	}
	for (int r = 0; r < rx; r++) {
		for (int c = 0; c < cx; c++) {
			POSITION pos = padd(cursor, (POSITION) { r, c });
			if (pos.x < 0 || pos.x >= MAP_HEIGHT || pos.y < 0 || pos.y >= MAP_WIDTH) continue;

			if (storm.exist) {
				int idx = get_storm_idx(pos);
				if (idx != -1) {
					printc(padd(map_pos, pos), storm.repr[idx], COLOR_CURSOR);
					continue;
				}
			}
			if (eagle.pos.x == pos.x && eagle.pos.y == pos.y) {
				printc(padd(map_pos, pos), eagle.repr, COLOR_CURSOR);
				continue;
			}
			int idx = get_unit_idx(pos);
			if (idx) { // ������ �����Ұ��
				printc(padd(map_pos, pos), units[idx].info_p->repr, COLOR_CURSOR);
				continue;
			}
			idx = get_sandworm_idx(pos);
			if (idx != 3) {
				printc(padd(map_pos, pos), sandworm[idx].info_p->repr, COLOR_CURSOR);
				continue;
			}
			idx = get_building_idx(pos);
			if (idx) { // �ǹ��� �����Ұ��
				char repr = (buildings[idx].info_p->repr == 's') ? '0' + buildings[idx].hp : buildings[idx].info_p->repr;
				printc(padd(map_pos, pos), repr, COLOR_CURSOR);
				continue;
			}
			if (plate[pos.x][pos.y] == 'B') {
				printc(padd(map_pos, pos), 'P', b_plate_info.color);
				continue;
			}
			if (plate[pos.x][pos.y] == 'R') {
				printc(padd(map_pos, pos), 'P', r_plate_info.color);
				continue;
			}
			printc(padd(map_pos, pos), map[pos.x][pos.y]->repr, COLOR_CURSOR);
		}
	}
}
bool check_cursor(POSITION pos) {
	int rx = 1, cx = 1;
	if (build_mode) {
		rx = 2;
		cx = 2;
	}
	for (int r = 0; r < rx; r++) {
		for (int c = 0; c < cx; c++) {
			if (pos.x == cursor.x + r && pos.y == cursor.y + c) return 1;
		}
	}
	return 0;
}

// ����� �Լ�
void display_map() {
	for (int r = 0; r < MAP_HEIGHT; r++) {
		for (int c = 0; c < MAP_WIDTH; c++) {
			if (map_change[r][c]) {
				if (storm.exist) {
					int idx = get_storm_idx((POSITION) { r, c });
					if (idx != -1) {
						int color = (check_cursor((POSITION) { r, c })) ? COLOR_CURSOR : storm.color;
						printc(padd(map_pos, (POSITION) { r, c }), storm.repr[idx], color); 
						map_change[r][c] = 0;
						continue; 
					}
				}
				if (eagle.pos.x == r && eagle.pos.y == c) {
					int color = (check_cursor((POSITION) { r, c })) ? COLOR_CURSOR : eagle.color;
					printc(padd(map_pos, (POSITION) { r, c }), eagle.repr, color);
					map_change[r][c] = 0;
					continue;
				}
				int idx = get_unit_idx((POSITION) { r, c });
				if (idx) { // ������ �����Ұ��
					int color;
					if (check_cursor((POSITION) { r, c })) {
						color = COLOR_CURSOR;
					}
					else if (map[r][c]->repr == 'R') { // ������ ������ �ö󰣰��, ȸ������ ǥ��
						if (units[idx].info_p->color == COLOR_BLUE) {
							color = 113;
						}
						else {
							color = 116;
						}
					}
					else {
						color = units[idx].info_p->color;
					}
					printc(padd(map_pos, (POSITION) { r, c }), units[idx].info_p->repr, color);
					map_change[r][c] = 0;
					continue;
				}
				idx = get_sandworm_idx((POSITION) { r, c });
				if (idx != 3) {
					int color = (check_cursor((POSITION) { r, c })) ? COLOR_CURSOR : sandworm[idx].info_p->color;
					printc(padd(map_pos, (POSITION) { r, c }), sandworm[idx].info_p->repr, color);
					map_change[r][c] = 0;
					continue;
				}
				idx = get_building_idx((POSITION) { r, c });
				if (idx) { // �ǹ��� �����Ұ��
					char repr = (buildings[idx].info_p->repr == 's') ? '0' + buildings[idx].hp : buildings[idx].info_p->repr;
					int color = (check_cursor((POSITION) { r, c })) ? COLOR_CURSOR : buildings[idx].info_p->color;
					printc(padd(map_pos, (POSITION) { r, c }), repr, color);
					map_change[r][c] = 0;
					continue;
				}
				if (plate[r][c] == 'B') { // ������ �����Ҷ�
					int color = (check_cursor((POSITION) { r, c })) ? COLOR_CURSOR : b_plate_info.color; 
					printc(padd(map_pos, (POSITION) { r, c }), 'P', color); 
					map_change[r][c] = 0;
					continue;
				}
				if (plate[r][c] == 'R') { // ������ �����Ҷ�
					int color = (check_cursor((POSITION) { r, c })) ? COLOR_CURSOR : r_plate_info.color;
					printc(padd(map_pos, (POSITION) { r, c }), 'P', color); 
					map_change[r][c] = 0;
					continue;
				}
				int color = (check_cursor((POSITION) { r, c })) ? COLOR_CURSOR : map[r][c]->color;
				printc(padd(map_pos, (POSITION) { r, c }), map[r][c]->repr, color);
				map_change[r][c] = 0;
			}
		}
	}
	//display_cursor(); 
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
	print_message(padd(sys_pos, pos), sys_log.arr[sys_log.cur]);
}

// ����â ����Լ�
void display_state_message() {
	for (int r = 0; r < STA_HEIGHT - 2; r++) {
		erase_message(padd(state_pos, (POSITION) { 2 + r, 0 }), STA_WIDTH - 1);
	}

	STATE_MESSAGE* message; 
	int idx = get_sandworm_idx(cursor); 
	if (idx != 3) {
		message = &sandworm[idx].info_p->state_message;
	}
	else if (selected_unit->info_p->repr) {
		message = &selected_unit->info_p->state_message;
	}
	else if (selected_building->info_p->repr) {
		message = &selected_building->info_p->state_message;
	}
	else if (plate[cursor.x][cursor.y]) {
		message = (plate[cursor.x][cursor.y] == 'B') ? &b_plate_info.state_message : &r_plate_info.state_message;
	}
	else {
		message = &map[cursor.x][cursor.y]->state_message;
	}

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

	// ���õ� �ǹ��� ����(���õ� �ǹ��� �������� error_b�� ����Ű�� �ְ�, error_b�� repr���� 0)
	if (selected_building->info_p->repr) {
		// �����̽��� ���, ���差�� ���
		if (selected_building->info_p->repr == 's') {
			gotoxy(padd(state_pos, pos));
			printf("���差 : %d", selected_building->hp);
			return;
		}
		else if (selected_building->hp > 0) {
			gotoxy(padd(state_pos, pos));
			printf("����ü�� : ");
			for (int i = 0; i < selected_building->hp / 10; i++) printf("@");
			if (selected_building->destroied) {
				printf(" (%d, ���ĵ�)", selected_building->hp);

			}
			else {
				printf(" (%d)", selected_building->hp);
			}
			if (selected_building->info_p->repr == 'B' && selected_building->info_p->color == COLOR_RED) { // ����� ������ ��� �����̽� �������� ǥ��
				pos.x += 2;
				gotoxy(padd(state_pos, pos));
				printf("%d �����̽� ������", r_resource.spice);
			}

			return;
		}
	}


	if (selected_unit->info_p->repr) { // ���õ� ������ ����(���õ� ������ �������� error_n�� ����Ű�� �ְ�, error_n�� repr���� 0)
		gotoxy(padd(state_pos, pos));
		printf("����ü�� : ");

		for (int i = 0; i < selected_unit->hp / 10; i++) printf("@");
		printf(" (%d)", selected_unit->hp);

		pos.x += 2; 
		gotoxy(padd(state_pos, pos)); 

		if (selected_unit->info_p->repr == 'H') { // �Ϻ������� ���
			if (selected_unit->mode == move_to_s || \
				selected_unit->mode == move_to_b|| \
				selected_unit->mode == move_to_b_w||\
				selected_unit->mode == wait_h) {
				printf("���� ���� : ��Ȯ(�����̽� %d�� ������)", selected_unit->havest_num); 
			}
			else if (selected_unit->mode == wait) { 
				printf("���� ���� : ���(�����̽� %d�� ������)", selected_unit->havest_num); 
			}
			else if (selected_unit->mode == move) {
				printf("���� ���� : �̵�(�����̽� %d�� ������)", selected_unit->havest_num); 
			}
		}
		else {
			if (selected_unit->mode == combat) {
				printf("���� ���� : ������");
			}
			else if (selected_unit->mode == wait) {  
				printf("���� ���� : ���");
			}
		}

	}

	if (idx != 3) { // ����� ����
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
void display_cmd_message() {
	for (int r = 0; r < CMD_HEIGHT - 2; r++) {
		erase_message(padd(cmd_pos, (POSITION) { 2 + r, 0 }), STA_WIDTH - 1);
	}

	CMD_MESSAGE* message = get_cmd_message(cursor);
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
	bool t = 1;
	display_resource(&t);

	for (int r = 0; r < MAP_HEIGHT; r++) {
		for (int c = 0; c < MAP_WIDTH; c++) {
			map_change[r][c] = 1;
		}
	}
	display_map(cursor);
}

// ���� ���
void esc(bool* build_ready) { 
	selected_unit = &units[0];
	selected_building = &buildings[0];

	for (int r = 0; r < STA_HEIGHT - 2; r++) {
		erase_message(padd(state_pos, (POSITION) { 2 + r, 0 }), STA_WIDTH - 1);
	}
	for (int r = 0; r < CMD_HEIGHT - 2; r++) {
		erase_message(padd(cmd_pos, (POSITION) { 2 + r, 0 }), STA_WIDTH - 1);
	}
	POSITION pos = { 2, 0 };
	print_message(padd(cmd_pos, pos), "B : Build");
	pos.x += 2;
	print_message(padd(cmd_pos, pos), "/ : command");

	// �Ǽ���� ����, Ŀ�� 1x1�� ����
	if (build_mode) {
		build_mode = 0;
		for (int r = 0; r < 2; r++) {
			for (int c = 0; c < 2; c++) {
				if (cursor.x < 0 || cursor.x >= MAP_HEIGHT - 1 || cursor.y < 0 || cursor.y >= MAP_WIDTH - 1) continue;
				map_change[cursor.x + r][cursor.y + c] = 1;
			}
		}
	}
	*build_ready = 0;
	strncpy_s(cmd_mode, 20, "off", 20);
}

// �ʱ� ���
void display() {
	bool t = 1;
	display_resource(&t);
	display_time();
	display_frame();
	display_map(); 
	POSITION pos = { 2, 0 };
	print_message(padd(cmd_pos, pos), "B : Build");
	pos.x += 2;
	print_message(padd(cmd_pos, pos), "/ : command");
}
// build
void display_build_list(bool *build_ready) {  
	// ����, �ǹ� �� ���� �� ���� ������ ����
	if (selected_unit->info_p->repr || selected_building->info_p->repr) return; 
	*build_ready = 1;

	for (int r = 0; r < CMD_HEIGHT - 2; r++) { 
		erase_message(padd(cmd_pos, (POSITION) { 2 + r, 0 }), STA_WIDTH - 1); 
	} 

	POSITION pos = { 2, 0 };
	print_message(padd(cmd_pos, pos), "P: plate");
	pos.y = CMD_WIDTH / 2;
	print_message(padd(cmd_pos, pos), "D: Dormitory");
	pos.x += 2;
	pos.y = 0; 
	print_message(padd(cmd_pos, pos), "G: Garage");  
	pos.y = CMD_WIDTH / 2; 
	print_message(padd(cmd_pos, pos), "B: Barracks"); 
	pos.x += 2; 
	pos.y = 0; 
	print_message(padd(cmd_pos, pos), "S: Shelter"); 
}
void display_cmd_list() {
	for (int r = 0; r < CMD_HEIGHT - 2; r++) { 
		erase_message(padd(cmd_pos, (POSITION) { 2 + r, 0 }), STA_WIDTH - 1); 
	}

	POSITION pos = { 2, 0 }; 
	print_message(padd(cmd_pos, pos), "1: ����� on/off");  
	pos.y = CMD_WIDTH / 2;
	print_message(padd(cmd_pos, pos), "2: ������ ���� ����"); 
	pos.x += 2;
	pos.y = 0;
	print_message(padd(cmd_pos, pos), "3: ����� ���� ����");
	pos.y = CMD_WIDTH / 2;
	print_message(padd(cmd_pos, pos), "4: �ڿ� ����");
	pos.x += 2;
	pos.y = 0;
	print_message(padd(cmd_pos, pos), "5: �����̽� ������ ����");
	pos.y = CMD_WIDTH / 2;
	print_message(padd(cmd_pos, pos), "6: �� ����");

}
void erase_cmd() {
	for (int r = 0; r < CMD_HEIGHT - 2; r++) {
		erase_message(padd(cmd_pos, (POSITION) { 2 + r, 0 }), STA_WIDTH - 1);
	}
}
void display_r_units_list() {
	for (int r = 0; r < CMD_HEIGHT - 2; r++) {
		erase_message(padd(cmd_pos, (POSITION) { 2 + r, 0 }), STA_WIDTH - 1); 
	}

	POSITION pos = { 2, 0 }; 
	print_message(padd(cmd_pos, pos), "1: �Ϻ�����"); 
	pos.y = CMD_WIDTH / 2; 
	print_message(padd(cmd_pos, pos), "2: ����"); 
	pos.x += 2; 
	pos.y = 0; 
	print_message(padd(cmd_pos, pos), "3: ������"); 
}