#ifndef _COMMON_H_  
#define _COMMON_H_

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <conio.h>
#include <assert.h>


/* ================= system parameters =================== */
#define TICK 10		// time unit(ms)


#define MAP_WIDTH	60
#define MAP_HEIGHT  16

// ��ĭ�� ���� ����ϱ� ������ ���ٿ� ����� �� �ִ� ũ��� �ʺ� - 1
#define SYS_WIDTH	MAP_WIDTH  
#define SYS_HEIGHT	MAP_HEIGHT / 2 

#define STA_WIDTH	MAP_WIDTH * 6/7 
#define STA_HEIGHT	MAP_HEIGHT 

#define CMD_WIDTH	STA_WIDTH 
#define CMD_HEIGHT	SYS_HEIGHT


/* ================= game data =================== */
#define OBJ_NUM 2
#define STR_NUM 3
#define MAX_TAIL 5
#define MAX_OBJ 10

/* ================= ��ġ�� ���� =================== */
// �ʿ��� ��ġ�� ��Ÿ���� ����ü
typedef struct {
	int x, y;
} POSITION;


// �Է� ������ Ű ����.
// �������� enum�� �����ߴµ�, ũ�� ����� ������ ���� �˻�
typedef enum {
	// k_none: �Էµ� Ű�� ����. d_stay(�� �����̴� ���)�� ����
	k_none = 0, k_up, k_right, k_left, k_down,
	k_quit,
	k_undef,	// ���ǵ��� ���� Ű �Է�	
	k_re_dis,	// �� �ٽ����
	k_space,
	k_esc,
	k_test,
	k_1,
	k_w,
	k_h
} KEY;


// DIRECTION�� KEY�� �κ�����������, �ǹ̸� ��Ȯ�ϰ� �ϱ� ���ؼ� �ٸ� Ÿ������ ����
typedef enum {
	d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION;


/* ================= ��ġ�� ����(2) =================== */
// ���Ǽ��� ���� �Լ���. KEY, POSITION, DIRECTION ����ü���� ���������� ��ȯ

// ���Ǽ� �Լ�
inline POSITION padd(POSITION p1, POSITION p2) {
	POSITION p = { p1.x + p2.x, p1.y + p2.y };
	return p;
}

// p1 - p2
inline POSITION psub(POSITION p1, POSITION p2) {
	POSITION p = { p1.x - p2.x, p1.y - p2.y };
	return p;
}

// ����Ű���� Ȯ���ϴ� �Լ�
#define is_arrow_key(k)		(k_up <= (k) && (k) <= k_down)

// ȭ��ǥ 'Ű'(KEY)�� '����'(DIRECTION)���� ��ȯ. ���� ���� �Ȱ����� Ÿ�Ը� �ٲ��ָ� ��
#define ktod(k)		(DIRECTION)(k)

// DIRECTION�� POSITION ���ͷ� ��ȯ�ϴ� �Լ�
inline POSITION dtop(DIRECTION d) {
	static POSITION direction_vector[] = { {0, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 0} };
	return direction_vector[d];
}

// p�� d �������� �̵���Ų POSITION
#define pmove(p, d)		(padd((p), dtop(d)))

/* ================= game data =================== */
typedef struct {
	int spice;		// ���� ������ �����̽�
	int spice_max;  // �����̽� �ִ� ���差
	int population; // ���� �α� ��
	int population_max;  // ���� ������ �α� ��
}RESOURCE;

// message
typedef
struct {
	int size;
	int about_size;
	char message[10][100];
}STATE_MESSAGE;

typedef
struct {
	int size;
	char message[3][100];
}CMD_MESSAGE;

// selection
typedef
struct {
	POSITION pos;
	char repr;
}SELECTION;

// ��ü ����
typedef
struct {
	char repr;
	char name[30];
	char mother;
	int color;
	int cost;
	int population;
	int damage;
	int max_hp;
	int move_period;
	int attack_period; // �����ֱ�
	STATE_MESSAGE state_message;
	CMD_MESSAGE cmd_message;
}UNIT_INFO;

typedef
struct {
	bool exist;
	POSITION pos;
	POSITION dest;
	int hp;
	int next_move_time;
	int next_attack_time;
	UNIT_INFO* info_p;
	unsigned char pre, next;
}UNIT;

typedef
struct {
	char repr;
	char name[20];
	int color;
	int cost;
	int size;
	int max_hp;
	STATE_MESSAGE state_message;
	CMD_MESSAGE cmd_message;
}BUILDING_INFO;

typedef
struct {
	bool exist;
	bool destroied;
	POSITION pos;
	int hp;
	BUILDING_INFO* info_p;
	unsigned char pre, next;
}BUILDING;

typedef
struct {
	char repr;
	int color;
	STATE_MESSAGE state_message;
}NATURE_INFO;


typedef
struct {
	POSITION pos;
	POSITION dest;
	int len;
	int hp;
	int next_move_time;
	int next_attack_time;
	int next_emission_time;
	UNIT_INFO* info_p;
}SANDWORM;

typedef
struct {
	char repr;
	int color;
	int move_period;
	int next_move_time;
	POSITION pos;
	POSITION dest;
	STATE_MESSAGE state_message;
}EAGLE;

typedef
struct {
	bool exist;
	POSITION pos;
	char repr[4];
	int color;
	int next_move_time;
	int exist_time;
	STATE_MESSAGE state_message;
}STORM;

#endif
