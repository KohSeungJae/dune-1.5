/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// ǥ���� ���� ����. ���� ���� �״µ�, ���ⲯ �߰��ϰų� �����ϱ�
#define COLOR_DEFAULT	15			// ����, �Ͼ�
#define COLOR_CURSOR	112			// �Ͼ�, ����
#define COLOR_RESOURCE  112			
#define COLOR_BULE		31			
#define COLOR_RED		79			
#define COLOR_BLACk		15
#define COLOR_DY		111
#define COLOR_GARY		143
#define COLOR_GREEN		47
#define COLOR_YELLOW	224


// ������ �ڿ�, ��, Ŀ���� ǥ��
// ������ ȭ�鿡 ǥ���� ����� ���⿡ �߰��ϱ�
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
