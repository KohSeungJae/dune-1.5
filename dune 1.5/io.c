#include "io.h"

void gotoxy(POSITION pos) { // Ŀ�� �̵��Լ�
	COORD coord = { pos.y, pos.x }; // ��, �� ������ �����ؾ���
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void set_color(int color) { // ���ٲٴ� �Լ�
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printc(POSITION pos, char ch, int color) { // �ش� ��ǥ�� �̵��� �ش� �������� ���ڸ� ����ϴ� �Լ�
	if (color >= 0) {
		set_color(color);
	}
	gotoxy(pos);
	printf("%c", ch);
}

KEY get_key(void) { // ����Ű ��ȯ�Լ�
	if (!_kbhit()) {  // �Էµ� Ű�� �ִ��� Ȯ��
		return k_none;
	}

	int byte = _getch();    // �Էµ� Ű�� ���� �ޱ�
	switch (byte) {
	case 'Q':
	case 'q': return k_quit;	// 'q'�� ������ ����
	case 'R':
	case 'r': return k_re_dis;  // 'r'�� ������ ���� �ٽ� ���
	case 'T':
	case 't': return k_test;    // �׽�Ʈ Ű
	case 'W':
	case 'w': return k_w;
	case 'H':
	case 'h': return k_h;
	case '1': return k_1;
	case 27: return k_esc;
	case 32: return k_space;
	case 224:
		byte = _getch();  // MSB 224�� �Է� �Ǹ� 1����Ʈ �� ���� �ޱ�
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
