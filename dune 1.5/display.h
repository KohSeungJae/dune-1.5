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
