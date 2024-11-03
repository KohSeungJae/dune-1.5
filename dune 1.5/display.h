/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// 표시할 색상 정의. 대충 맞춰 뒀는데, 취향껏 추가하거나 변경하기
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


void display_resource(RESOURCE resource);

void display_frame();
void display_time();
void display_map(POSITION cursor);

void print_message(POSITION pos, char str[]);
void erase_message(POSITION pos, int size);
void display_system_message(char new_str[]);
void re_display(RESOURCE resource, POSITION cursor, bool chage_map[MAP_HEIGHT][MAP_WIDTH]);
void dispaly_state_message(POSITION selection_pos);
void display_cmd_message(POSITION selection_pos);
void esc(SELECTION*);
void display_system_message(char new_str[]);

#endif
#pragma once
