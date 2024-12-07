#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "display.h"
#include "ai.h"


extern POSITION Q[MAP_HEIGHT * MAP_WIDTH];
extern int dist[MAP_HEIGHT][MAP_WIDTH];

extern int sys_clock;
extern POSITION cursor;
extern SANDWORM sandworm[2];
extern UNIT units[100];
extern BUILDING buildings[100];
extern bool map_change[MAP_HEIGHT][MAP_WIDTH];
extern NATURE_INFO* map[MAP_HEIGHT][MAP_WIDTH];
extern char plate[MAP_HEIGHT][MAP_WIDTH];
extern 
UNIT_INFO
error_n,
b_havester_info,
r_havester_info,
b_soldier_info,
b_fremen_info,
r_fighter_info,
r_tank_info,
sandworm_info;

extern
BUILDING_INFO
error_b,
b_base_info,
r_base_info,
b_plate_info,
r_plate_info,
spice_info,
b_dormitory_info,
b_garage_info,
b_barracks_info,
b_shelter_info;

RESOURCE r_resource = {
	.spice = 0,
	.spice_max = 20,
	.population = 5,
	.population_max = 10
};

typedef
enum {
	get_spice,
	ready,
	build_up,

}AI_STATE; 
AI_STATE ai_state = 0;

void get_spice_state();
void ready_state(); 
int get_r_unit_idx(char repr) {
	int idx = 0;
	while (units[units[idx].next].exist) {
		idx = units[idx].next;
		if (units[idx].info_p->color == COLOR_RED && units[idx].info_p->repr == repr &&\
			units[idx].mode == wait) {
			return idx;
		}
	}
}
POSITION get_r_building_idx(char repr, POSITION pos) { // ���� ����� repr�� ã��  
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
			if (map[npos.x][npos.y]->repr == 'R' || get_unit_idx(npos)) continue;
			int idx = get_building_idx(npos); 
			if (idx && buildings[idx].info_p->repr == 's') {
				return npos;
			}

			dist[npos.x][npos.y] = dist[pos.x][pos.y] + 1;
			Q[tail++] = npos;
		}
	}
	return (POSITION) { -1, -1 };
}
POSITION find_r_base(POSITION pos) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			dist[i][j] = -1;
		}
	}

	// ���� ��ġ
	POSITION b_pos[4] = { {0, MAP_WIDTH - 2}, {0, MAP_WIDTH - 1}, {1, MAP_WIDTH - 2}, {1, MAP_WIDTH - 1} };
	
	int head = 0, tail = 0;
	Q[tail++] = pos;
	dist[pos.x][pos.y] = 0;


	while (head != tail) {
		POSITION pos = Q[head++];

		for (int i = 1; i <= 4; i++) {
			POSITION npos = pmove(pos, i);

			if (npos.x < 0 || npos.x >= MAP_HEIGHT || npos.y < 0 || npos.y >= MAP_WIDTH || dist[npos.x][npos.y] != -1) continue;
			if (map[npos.x][npos.y]->repr == 'R' || get_unit_idx(npos)) continue;
			for (int i = 0; i < 4; i++) {
				if (npos.x == b_pos[i].x && npos.y == b_pos[i].y) {
					return pos; // pos(���� �� ��ġ)�� ����
				}
			}

			dist[npos.x][npos.y] = dist[pos.x][pos.y] + 1;
			Q[tail++] = npos;
		}
	}
}
void build_up_state(); 

void ai_engine() {
	switch (ai_state) {
		case get_spice: get_spice_state(); break; 
		case ready: ready_state(); break;
		case build_up: build_up_state(); break;

	}



}

void get_spice_state() {
	// get_spice ���� - �����̽� ä�����
	if (sys_clock < 2000) return; // ���ӽ��� 4���� ����� ����

	int idx = get_r_unit_idx('H');
	POSITION pos = get_r_building_idx('s', units[idx].pos);
	int b_idx = get_building_idx(pos);

	units[idx].point2 = buildings[b_idx].pos; 
	units[idx].dest = buildings[b_idx].pos; 
	units[idx].next_move_time = sys_clock + units[idx].info_p->move_period; 
	units[idx].mode = move_to_s;

	ai_state = ready; 
}

void r_havest(UNIT* unit) { 
	unit->next_move_time = sys_clock + unit->info_p->move_period / 5; // �̵� Ȱ��ȭ  
	unit->dest = get_position((POSITION) { 0, MAP_WIDTH-2 });  // ������ ����   

	for (DIRECTION dir = 1; dir <= 4; dir++) {
		POSITION pos = padd(unit->pos, dtop(dir));
		int idx = get_building_idx(pos);
		if (buildings[idx].info_p->repr == 's') {
			unit->havest_num = rand() % 2 + 2;
			if (buildings[idx].hp - unit->havest_num <= 0) {
				unit->havest_num = buildings[idx].hp;
				buildings[idx].hp = 0;
				building_erase(pos);
				display_system_message("�ش� �����̽��� �ٴڳ����ϴ�!");
				unit->mode = move_to_b_w; // ���̽��� �̵��� ����϶�� �ǹ� 
			}
			else {
				buildings[idx].hp -= unit->havest_num;
				unit->mode = move_to_b; // ���̽��� �̵����̶�� �ǹ�  
			}

			map_change[pos.x][pos.y] = 1;
			return;
		}
	}
	// �����̽��� ���������
	unit->mode = move_to_b_w; // ���̽��� �̵����̶�� �ǹ� 
}
void r_move_to_base(UNIT* unit, char mode) {
	if (r_resource.spice + unit->havest_num > r_resource.spice_max) { 
		r_resource.spice = r_resource.spice_max; 
		display_system_message("�����̽��� ���� á���ϴ�."); 
		unit->mode = wait; 
		return;
	}
	else {
		r_resource.spice += unit->havest_num;
	}

	if (mode == 'w') {
		unit->mode = wait; 
	}
	else {
		unit->dest = unit->point2; 
		unit->mode = move_to_s;
		unit->next_move_time = sys_clock + unit->info_p->move_period; // �̵� Ȱ��ȭ    
	}
	unit->havest_num = 0; 
}
void ready_state() { 
	static bool new_havester = 0;
	if (r_resource.spice >= 5) { // �Ϻ����� ����
		POSITION pos = get_position((POSITION) { 0, MAP_WIDTH - 2 }); 
		r_resource.spice -= r_havester_info.cost;
		r_resource.population += r_havester_info.population;
		unit_push(&r_havester_info, pos);
		map_change[pos.x][pos.y] = 1;
		new_havester = 1;
	}
	if (new_havester) { // ������� ���Ǹ� ���� �Ϻ�����
		int idx = get_r_unit_idx('H');
		units[idx].mode = move_to_sw;
		units[idx].combat_pos_p = &sandworm[1];
		units[idx].next_move_time = sys_clock + units[idx].info_p->move_period;
		ai_state = build_up;
	}
}
void build_up_state() {
	int idx = get_r_unit_idx('H');
	POSITION pos = get_r_building_idx('s', units[idx].pos);
	int b_idx = get_building_idx(pos);
	if (pos.x != -1) {
		units[idx].point2 = buildings[b_idx].pos; 
		units[idx].dest = buildings[b_idx].pos; 
		units[idx].next_move_time = sys_clock + units[idx].info_p->move_period; 
		units[idx].mode = move_to_s; 
	}

}