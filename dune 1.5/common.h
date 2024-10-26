#ifndef _COMMON_H_  
#define _COMMON_H_

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <conio.h>
#include <assert.h>


/* ================= system parameters =================== */
#define TICK 10		// time unit(ms)

#define N_LAYER 3
#define N_C_LAYER 4  // 최상위 레이어는 커서
#define MAP_WIDTH	60
#define MAP_HEIGHT	18

#define SYS_WIDTH	MAP_WIDTH 
#define SYS_HEIGHT	MAP_HEIGHT / 2

#define STA_WIDTH	MAP_WIDTH * 6/7
#define STA_HEIGHT	MAP_HEIGHT

#define CMD_WIDTH	STA_WIDTH 
#define CMD_HEIGHT	SYS_HEIGHT

/* ================= game data =================== */
#define OBJ_NUM 1
#define MAX_TAIL 5

/* ================= 위치와 방향 =================== */
// 맵에서 위치를 나타내는 구조체
typedef struct {
	int x, y;
} POSITION, CURSOR; 



// 입력 가능한 키 종류.
// 수업에서 enum은 생략했는데, 크게 어렵지 않으니 예제 검색
typedef enum {
	// k_none: 입력된 키가 없음. d_stay(안 움직이는 경우)에 대응
	k_none = 0, k_up, k_right, k_left, k_down,
	k_quit,
	k_undef,	// 정의되지 않은 키 입력	
	k_re_dis,	// 맵 다시출력
	k_space,
	k_esc,
	k_test,
	k_1, 
	k_w,
	k_h
} KEY;


// DIRECTION은 KEY의 부분집합이지만, 의미를 명확하게 하기 위해서 다른 타입으로 정의
typedef enum {
	d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION;


/* ================= 위치와 방향(2) =================== */
// 편의성을 위한 함수들. KEY, POSITION, DIRECTION 구조체들을 유기적으로 변환

// 편의성 함수
inline POSITION padd(POSITION p1, POSITION p2) {
	POSITION p = { p1.x + p2.x, p1.y + p2.y };
	return p;
}

// p1 - p2
inline POSITION psub(POSITION p1, POSITION p2) {
	POSITION p = { p1.x - p2.x, p1.y - p2.y };
	return p;
}

// 방향키인지 확인하는 함수
#define is_arrow_key(k)		(k_up <= (k) && (k) <= k_down)

// 화살표 '키'(KEY)를 '방향'(DIRECTION)으로 변환. 정수 값은 똑같으니 타입만 바꿔주면 됨
#define ktod(k)		(DIRECTION)(k)

// DIRECTION을 POSITION 벡터로 변환하는 함수
inline POSITION dtop(DIRECTION d) {
	static POSITION direction_vector[] = { {0, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 0} };
	return direction_vector[d];
}

// p를 d 방향으로 이동시킨 POSITION
#define pmove(p, d)		(padd((p), dtop(d)))

/* ================= game data =================== */
typedef struct {
	int spice;		// 현재 보유한 스파이스
	int spice_max;  // 스파이스 최대 저장량
	int population; // 현재 인구 수
	int population_max;  // 수용 가능한 인구 수
} RESOURCE;


// 유닛 구조체
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
}ORDER_MESSAGE;

typedef struct {
	int layer;
	POSITION pos;		// 현재 위치(position)
	POSITION dest;		// 목적지(destination)
	char repr;			// 화면에 표시할 문자(representation)
	int color;
	int len;
	int speed;			// 움직이는 주기
	int next_move_time;	// 다음에 움직일 시간
	STATE_MESSAGE state_message;
	ORDER_MESSAGE order_message;
}OBJECT;

typedef struct { // 20바이트
	int hp;
	POSITION pos;
	POSITION dest; 
}PRIVATE;


typedef struct {
	char mother;
	int cost;
	int population; 
	int size;
	int layer;
	char repr;			
	int color;
	int speed;			
	int next_move_time;	

	STATE_MESSAGE state_message;
	ORDER_MESSAGE order_message;

	PRIVATE object[10];  // 각 유닛 정보
}OBJECTS;

typedef struct {
	char repr;
	int cost;
	int hp;
	int size;     // 한변의 길이
	POSITION pos; // 좌상단 좌표

	STATE_MESSAGE state_message; 
	ORDER_MESSAGE order_message;

	PRIVATE structure[10];
}STRUCTURE;



#endif
