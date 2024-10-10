/*
* raw(?) I/O
*/
#include "io.h"

void gotoxy(POSITION pos) { // 커서 이동함수
	COORD coord = { pos.column, pos.row }; // 열, 행 순서로 전달해야함
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void set_color(int color) { // 색바꾸는 함수
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printc(POSITION pos, char ch, int color) { // 해당 좌표로 이동후 해당 색상으로 문자를 출력하는 함수
	if (color >= 0) {
		set_color(color);
	}
	gotoxy(pos);
	printf("%c", ch);
}

KEY get_key(void) { // 방향키 반환함수
	if (!_kbhit()) {  // 입력된 키가 있는지 확인
		return k_none;
	}

	int byte = _getch();    // 입력된 키를 전달 받기
	switch (byte) {
	case 'q': return k_quit;	// 'q'를 누르면 종료
	case 'r': return k_re_dis;  // 'r'를 누르면 맵을 다시 출력
	case 't': return k_test;    // 테스트 키
	case '1': return k_1;
	case'2':  return k_2;
	case 224:
		byte = _getch();  // MSB 224가 입력 되면 1바이트 더 전달 받기
		switch (byte) {
		case 72: return k_up;
		case 75: return k_left;
		case 77: return k_right;
		case 80: return k_down;
		default: return k_undef;
		}
	default: return k_undef;
	}
}
