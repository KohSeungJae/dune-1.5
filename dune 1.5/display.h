#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

#define COLOR_DEFAULT	15			// 검정, 하양
#define COLOR_WHITE		240
#define COLOR_CURSOR	47			// 초록
#define COLOR_RESOURCE  112			// 흰 검
#define COLOR_BLUE		31			
#define COLOR_RED		79			
#define COLOR_BLACk		15
#define COLOR_DY		111
#define COLOR_GARY		143
#define COLOR_GREEN		47
#define COLOR_YELLOW	224
#define COLOR_PURPLE	223


void display_map();
void display_frame();
void display_resource(bool* resource_change);
void display_time();
void display();

int get_building_idx(POSITION pos);
int get_unit_idx(POSITION pos);
int get_sandworm_idx(POSITION pos);

void print_message(POSITION pos, char str[]);
void erase_message(POSITION pos, int size);

void display_system_message(char new_str[]);
void display_state_message(); 
void display_cmd_message();
void re_display();
void esc(bool* build_ready);
void display_build_list(bool* build_ready); 
void display_cmd_list(); 
void erase_cmd(); 
void display_r_units_list(); 

#endif
#pragma once