#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "display.h"


extern int sys_clock;
extern POSITION cursor;
extern SANDWORM sandworm[2];
extern UNIT units[100];
extern BUILDING buildings[100];
extern bool map_change[MAP_HEIGHT][MAP_WIDTH];
extern NATURE_INFO* map[MAP_HEIGHT][MAP_WIDTH];
extern char plate[MAP_HEIGHT][MAP_WIDTH];
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
	.spice = 20,
	.spice_max = 20,
	.population = 5,
	.population_max = 10
};

typedef
enum {
	ready,
	build_up,

}AI_STATE; 
AI_STATE ai_state = 0;

void ready_state();
void r_havest() {

}

void ai_engine() {
	switch (ai_state) {
		case 1: break;

	}



}

void ready_state() {
	// ready 상태 - 스파이스 채집, 샌드웜 주의 끌기

	// 스파이스 채집
	if (r_resource.spice == 0) {

	}


}