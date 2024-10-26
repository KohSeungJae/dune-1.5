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
#define COLOR_WHITE		240
#define COLOR_CURSOR	47			// �ʷ�
#define COLOR_RESOURCE  112			// �� ��
#define COLOR_BULE		31			
#define COLOR_RED		79			
#define COLOR_BLACk		15
#define COLOR_DY		111
#define COLOR_GARY		143
#define COLOR_GREEN		47
#define COLOR_YELLOW	224
#define COLOR_PURPLE	223



void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], 
	CURSOR cursor
);

void re_display(RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor);

void display_frame(); 
void display_system_message(char new_str[]);
void display_object_info(CURSOR); 
void display_command(CURSOR);
void select_object(CURSOR cursor);


void esc();


#endif
