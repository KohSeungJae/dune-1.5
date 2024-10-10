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
#define COLOR_CURSOR	112			// 하양, 검정
#define COLOR_RESOURCE  112			
#define COLOR_BULE		31			
#define COLOR_RED		79			
#define COLOR_BLACk		15
#define COLOR_DY		111
#define COLOR_GARY		143
#define COLOR_GREEN		47
#define COLOR_YELLOW	224


// 지금은 자원, 맵, 커서만 표시
// 앞으로 화면에 표시할 내용들 여기에 추가하기
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor
);

void dispaly_frame(); 
void display_system_frame();
void display_state_frame();
void display_order_frame();
void display_system_message(char new_str[]);
void insert_system_str(char new_str[]);


void re_display(RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor);



 


#endif
