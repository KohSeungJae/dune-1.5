#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"
#include "ai.h"

/* ================= control =================== */
int sys_clock = 0;
POSITION cursor = { 0, 0 };
RESOURCE resource = {
	.spice = 0,
	.spice_max = 20,
	.population = 5,
	.population_max = 10
};

// 둘이 동시에 존재할 수 없음
UNIT* selected_unit; // 선택된 유닛
BUILDING* selected_building; // 선택된 건물


/* ================= game data =================== */
// 공유하지 않는 정보들 ex) 현재위치, 현재 체력, 다음 움직일 시간...
SANDWORM sandworm[2];
UNIT units[100] = { 0 }; // 유닛 연결 리스트
BUILDING buildings[100] = { 0 }; // 빌딩 연결 리스트
int unit_unused = 1;
int building_unused = 1;

bool map_change[MAP_HEIGHT][MAP_WIDTH] = { 0 };
NATURE_INFO* map[MAP_HEIGHT][MAP_WIDTH];
char plate[MAP_HEIGHT][MAP_WIDTH] = { 0 };

// 공유하는 정보들 ex) 최대체력, 속도....
UNIT_INFO
error_n = {
	.repr = '\0',
	.name = "유닛 에러",
	.color = 0,
	.cost = 5,
	.population = 5,
	.max_hp = 70,
	.move_period = 2000,
	.state_message = {
		.size = 1,
		.about_size = 1,
		.message = {
			"유닛 에러"
		}
	},
	.cmd_message = {
		.size = 1,
		.message = {
			"유닛 에러"
		}
	}
},
b_havester_info = {
	.repr = 'H',
	.name = "하베스터",
	.mother = 'B',
	.color = COLOR_BLUE,
	.cost = 5,
	.population = 5,
	.max_hp = 70,
	.move_period = 2500,
	.action_period = 2000,
	.state_message = {
		.size = 8,
		.about_size = 2,
		.message = {
			"[하베스터] (아트레이디스)",
			"스파이스를 수확해 배달한다.",
			"생산 비용 : 5",
			"인구수 : 5",
			"이동 주기 : 2초",
			"공격력 : 0",
			"체력 : 70",
			"시야 : 0",
		}
	},
	.cmd_message = {
		.size = 3,
		.message = {
			"H: 스파이스 채집",
			"M: 이동",
			"S : 저장"
		}
	}
},
r_havester_info = {
	.repr = 'H',
	.name = "하베스터",
	.color = COLOR_RED,
	.cost = 5,
	.population = 5,
	.max_hp = 70,
	.move_period = 2000,
	.action_period = 2000,
	.state_message = {
		.size = 8,
		.about_size = 2,
		.message = {
			"[하베스터] (하코넨)",
			"스파이스를 수확해 배달한다.",
			"생산 비용 : 5",
			"인구수 : 5",
			"이동 주기 : 2초",
			"공격력 : 0",
			"체력 : 70",
			"시야 : 0",
		}
	}
},
b_soldier_info = {
	.repr = 'S',
	.name = "보병",
	.mother = 'B',
	.color = COLOR_BLUE,
	.cost = 1,
	.population = 1,
	.max_hp = 15,
	.move_period = 1000,
	.action_period = 800,
	.damage = 5,
	.sight = 1,
	.state_message = {
		.size = 8,
		.about_size = 2,
		.message = {
			"[보병] (아트레이디스)",
			"가장 기본적인 전투유닛이다.",
			"생산 비용 : 1",
			"인구수 : 1",
			"이동 주기 : 1초",
			"공격력 : 5",
			"체력 : 15",
			"시야 : 1",
		}
	},
	.cmd_message = {
		.size = 2,
		.message = {
			"M: 이동",
			"P: 순찰"
		}
	}
},
b_fremen_info = { 
	.repr = 'F',
	.name = "프레멘",
	.mother = 'S',
	.color = COLOR_BLUE,
	.cost = 5,
	.population = 2,
	.max_hp = 25,
	.move_period = 400,
	.action_period = 200,
	.damage = 15,
	.sight = 8,
	.state_message = {
		.size = 8,
		.about_size = 2,
		.message = {
			"[프레멘] (아트레이디스)",
			"고급 전투 자원.",
			"생산 비용 : 5",
			"인구수 : 2",
			"이동 주기 : 0.4초",
			"공격력 : 15",
			"체력 : 25",
			"시야 : 8",
		}
	},
	.cmd_message = {
		.size = 2,
		.message = {
			"M: 이동",
			"P: 순찰"
		}
	} 
},
r_fighter_info = {
	.repr = 'F',
	.name = "투사",
	.mother = 'B',
	.color = COLOR_RED,
	.cost = 1,
	.population = 1,
	.max_hp = 10,
	.move_period = 1200,
	.action_period = 600,
	.damage = 6,
	.sight = 1,
	.state_message = {
		.size = 8,
		.about_size = 2,
		.message = {
			"[투사] (하코넨)",
			"가장 기본적인 전투유닛이다.",
			"생산 비용 : 1",
			"인구수 : 1",
			"이동 주기 : 1.2초",
			"공격력 : 6",
			"체력 : 15",
			"시야 : 1",
		}
	},
	.cmd_message = {
		.size = 2,
		.message = {
			"M: 이동",
			"P: 순찰"
		}
	}
},
r_tank_info = {
	.repr = 'T',
	.name = "투사",
	.mother = 'B',
	.color = COLOR_RED,
	.cost = 12,
	.population = 5,
	.max_hp = 60,
	.move_period = 3000,
	.action_period = 600,
	.damage = 40,
	.sight = 4,
	.state_message = {
		.size = 8,
		.about_size = 2,
		.message = {
			"[중전차] (하코넨)",
			"튼튼하고 강력한 전차.",
			"생산 비용 : 12",
			"인구수 : 5",
			"이동 주기 : 3초",
			"공격력 : 40",
			"체력 : 60",
			"시야 : 4",
		}
	},
	.cmd_message = {
		.size = 2,
		.message = {
			"M: 이동",
			"P: 순찰"
		}
	}
},
sandworm_info = {
	.repr = 'W',
	.name = "샌드웜",
	.color = COLOR_DY,
	.damage = 9999999,
	.max_hp = 9999999,
	.move_period = 2500,
	.action_period = 10000,
	.state_message = {
		.size = 4,
		.about_size = 4,
		.message = {
			"[샌드웜]",
			"사막의 무시무시한 포식자",
			"바위 위는 공격하지 못하는 듯 하다.",
			"가끔씩 스파이스를 배출한다."
		}
	}
};

BUILDING_INFO
error_b = {
	.repr = '\0',
	.name = "건물 에러",
	.color = 0,
	.cost = 0,
	.size = 2,
	.max_hp = 50,
	.state_message = {
		.size = 1,
		.about_size = 1,
		.message = {
			"건물 에러"
		}
	},
	.cmd_message = {
		.size = 1,
		.message = {
			"건물 에러"
		}
	}
},
b_base_info = {
	.repr = 'B',
	.name = "본진",
	.color = COLOR_BLUE,
	.cost = 0,
	.size = 2,
	.max_hp = 50,
	.state_message = {
		.size = 2,
		.about_size = 2,
		.message = {
			"[본진] (아트레이디스)",
			"하베스터를 생산 할 수 있다."
		}
	},
	.cmd_message = {
		.size = 1,
		.message = {
			"H : 하베스터 생산"
		}
	}
},
r_base_info = {
	.repr = 'B',
	.name = "본진",
	.color = COLOR_RED,
	.cost = 0,
	.size = 2,
	.max_hp = 50,
	.state_message = {
		.size = 2,
		.about_size = 2,
		.message = {
			"[본진] (하코넨)",
			"하베스터를 생산 할 수 있다."
		}
	}
},
b_plate_info = {
	.repr = 'P',
	.name = "장판",
	.color = 1,
	.cost = 1,
	.size = 2,
	.max_hp = -1,
	.state_message = {
		.size = 3,
		.about_size = 2,
		.message = {
			"[장판] (아트레이디스)",
			"위에 건물을 건설 할 수 있다.",
			"건설 비용 : 1"
		}
	}
},
r_plate_info = {
	.repr = 'P',
	.name = "장판",
	.color = 4,
	.cost = 1,
	.size = 2,
	.max_hp = -1,
	.state_message = {
		.size = 2,
		.about_size = 2,
		.message = {
			"[장판](하코넨)",
			"위에 건물을 건설 할 수 있다."
		}
	}
},
spice_info = {
	.repr = 's',
	.name = "스파이스",
	.color = 207,
	.cost = 0,
	.size = 1,
	.max_hp = 9,
	.state_message = {
		.size = 3,
		.about_size = 3,
		.message = {
			"[스파이스]",
			"온통 모래뿐인 사막에서의 귀중한 자원이다.",
			"하베스터를 통해 얻을 수 있다.",
		}
	}
},
b_dormitory_info = {
	.repr = 'D',
	.name = "숙소",
	.color = COLOR_BLUE,
	.cost = 2,
	.size = 2,
	.max_hp = 10,
	.state_message = {
		.size = 3,
		.about_size = 2,
		.message = {
			"[숙소]",
			"인구 최대치를 늘려준다.",
			"건설 비용 : 2",
		}
	}
},
b_garage_info = {
	.repr = 'G',
	.name = "창고",
	.color = COLOR_BLUE,
	.cost = 4,
	.size = 2,
	.max_hp = 10,
	.state_message = {
		.size = 3,
		.about_size = 2,
		.message = {
			"[창고]",
			"스파이스 최대 보유량을 늘려준다.",
			"건설 비용 : 4",
		}
	}
},
b_barracks_info = {
	.repr = 'B',
	.name = "병영",
	.color = 20, // 파랑, 빨강
	.cost = 4,
	.size = 2,
	.max_hp = 20,
	.state_message = {
		.size = 3,
		.about_size = 2,
		.message = {
			"[병영]",
			"보병을 훈련할 수 있는 건물이다.",
			"건설 비용 : 4",
		}
	},
	.cmd_message = {
		.size = 1,
		.message = {
			"S : 보병 생산"
		}
	}
},
b_shelter_info = {
	.repr = 'S',
	.name = "은신처",
	.color = COLOR_BLUE,
	.cost = 5,
	.size = 2,
	.max_hp = 30,
	.state_message = {
		.size = 3,
		.about_size = 2,
		.message = {
			"[병영]",
			"특수 유닛을 훈련할 수 있는 건물이다.",
			"건설 비용 : 5",
		}
	},
	.cmd_message = {
		.size = 1,
		.message = {
			"F : 프레멘 생산"
		}
	}
};

NATURE_INFO
desert_info = {
	.repr = ' ',
	.color = COLOR_YELLOW,
	.state_message = {
		.size = 3,
		.about_size = 3,
		.message = {
			"[사막 지형]",
			"물 한 방울도 찾아볼 수 없다.",
			"모래 속에는 무엇인가 살고 있는 것 같다..."
			}
	}
},
rock_info = {
	.repr = 'R',
	.color = COLOR_GARY,
	.state_message = {
		.size = 2,
		.about_size = 2,
		.message = {
			"[바위]",
			"평범한 바위이다."
		}
	}
};


EAGLE eagle = {
	.pos = {4, 0},
	.dest = {5, MAP_WIDTH - 1},
	.move_period = 500,
	.color = COLOR_PURPLE,
	.repr = 'E',
	.state_message = {
		.about_size = 2,
		.size = 2,
		.message = {
			"[사막독수리]",
			"이 사막에서 가장 평화로운 존재이다."
		}
	}
};
STORM storm = {
	.exist = 0,
	.pos = {MAP_HEIGHT - 2, MAP_WIDTH / 2},
	.repr = "STOM",
	.color = COLOR_DY,
	.next_move_time = 5000,
	.exist_time = 5000,
	.state_message = {
		.size = 3,
		.about_size = 3,
		.message = {
			"[모래폭풍]",
			"사나운 모래폭풍",
			"주기적으로 생성된다."
		}
	}
};

BUILDING plate_st = {
	.info_p = &b_plate_info
};


/* ================= utility =================== */
POSITION Q[MAP_HEIGHT * MAP_WIDTH];
int dist[MAP_HEIGHT][MAP_WIDTH];

int double_click_distance = 5;
int vist_timer = -1;
KEY prev_key;

bool sand_worm_on = 1;
bool unit_exist = 1;
bool storm_on = 1;

bool build_ready = 0;
bool build_mode = 0;

BUILDING_INFO* build_info;
bool resource_change;
char cmd_mode[20];
bool f_ready = 0;
// test
bool storm_test_on = 0;
void test_storm();
void make_fighter();
// 인트로 & 종료
void intro();
void outro();
// 유닛 & 빌딩을 연결리스트에 추가
void unit_push(UNIT_INFO* info, POSITION pos);
void unit_erase(POSITION pos);
void building_push(BUILDING_INFO* info, POSITION pos);
void building_erase(POSITION pos);
// init
inline void init_rock(POSITION pos);
inline void init_4rock(POSITION pos);
inline void init_total_rock();
void init();
// 커서 이동
void cursor_move(DIRECTION dir, int n);
// 선택
char get_repr(POSITION pos);
void select_object();
// 유닛공격
void attack_unit(UNIT_INFO* attacker, UNIT* victim);
// 샌드웜 관련
bool set_sandworm_dest(SANDWORM* sandworm);
int find_min_dist_s(POSITION pos, POSITION dest);
DIRECTION get_min_dir_s(POSITION pos, POSITION dest);
void sandworm_move(SANDWORM* sandworm);
void sandworm_emission(SANDWORM* sandworm);
void sandworm_on_off() {
	if (sand_worm_on) {
		sand_worm_on = 0;
		display_system_message("샌드웜 off");
	}
	else {
		sand_worm_on = 1;
		display_system_message("샌드웜 on");
	}
}
// 유닛 생성 함수
POSITION get_position(POSITION building_pos);
void make_unit(UNIT_INFO* unit_info);
// 독수리&모래폭풍
void eagle_move(EAGLE* eagle);
void storm_move();
void storm_move_test();
void storm_action();
// build
void build_mode_exchange(BUILDING_INFO* info);
void build();
// 하베스터
void harvest_mode_exchange();
void set_havest_dest();
void spice_save();
// 이동
void move_mode_exchange();
void set_selected_unit_dest();
// 순찰
void patrol_move_exchange();
// 전투
void combat_mode_exchange(UNIT* unit);
void unit_search();
// inline
inline space_action();
inline void objects_move();
void units_action();
// 테스트를 위한 명령어
void test_cmd(unsigned char cmd_num) { 
	if (cmd_num == 0) {
		strncpy_s(cmd_mode, 20, "on", 20);
		display_cmd_list(); 
		return;
	}

	if (strncmp(cmd_mode, "on", 20) == 0) {
		if (cmd_num == 1) {
			sandworm_on_off(); 
			strncpy_s(cmd_mode, 20, "off", 20); 
			esc(&build_ready); // 명령모드 종료
		}
		else if (cmd_num == 2) {
			display_r_units_list();  
			strncpy_s(cmd_mode, 20, "make_r_unit", 20); 
			// 종료 x
		}
		else if (cmd_num == 5) {
			display_system_message("위치를 선택해 주세요.");
			strncpy_s(cmd_mode, 20, "make_spice_bd", 20); 
		}
		else if (cmd_num == 6) {
			display_system_message("위치를 선택해 주세요.");
			strncpy_s(cmd_mode, 20, "make_rock", 20);
		}
	}
	else if (strncmp(cmd_mode, "make_r_unit", 20) == 0) {
		if (cmd_num == 1) {
			strncpy_s(cmd_mode, 20, "make_r_h", 20);
		}
		else if (cmd_num == 2) {
			strncpy_s(cmd_mode, 20, "make_r_f", 20);
		}
		else if (cmd_num == 3) {
			strncpy_s(cmd_mode, 20, "make_r_t", 20);
		}
		else {
			display_system_message("잘못된 번호입니다.");
			return;
		}
		display_system_message("위치를 선택해 주세요."); 
	}
}
void make_test_object() {
	if (strncmp(cmd_mode, "make_r_h", 20) == 0) {
		unit_push(&r_havester_info, cursor);
	}
	else if (strncmp(cmd_mode, "make_r_f", 20) == 0) {
		unit_push(&r_fighter_info, cursor); 
	}
	else if (strncmp(cmd_mode, "make_r_t", 20) == 0) {
		unit_push(&r_tank_info, cursor);
	}
	else if (strncmp(cmd_mode, "make_spice_bd", 20) == 0) {
		building_push(&spice_info, cursor);
	}
	else if (strncmp(cmd_mode, "make_rock", 20) == 0) {
		map[cursor.x][cursor.y] = &rock_info;
		map_change[cursor.x][cursor.y] = 1;
	}
	else {
		return;
	}

	esc(&build_ready); // 명령모드 종료
}



int main(void) { 
	srand((unsigned int)time(NULL));  
	intro();  
	init();  
	display();  

	while (1) {
		KEY key = get_key();

		if (vist_timer != -1 && vist_timer > 80) { // 80ms 이내에 다시 클릭이 이뤄지지 않았을때 
			cursor_move(ktod(prev_key), 1);
		}
		// 방향키 입력이 있으면 처리
		if (is_arrow_key(key)) {
			if (vist_timer == -1) {
				vist_timer = 0;
				prev_key = key;
			}
			else {
				if (prev_key == key)
					cursor_move(ktod(key), double_click_distance);
				else
					cursor_move(ktod(key), 1);
			}
		}
		else {
			// 방향키 외의 입력
			switch (key) {
			case k_space: space_action();  break;
			case k_h:
				make_unit(&b_havester_info);
				harvest_mode_exchange();
				break;
			case k_re_dis: re_display(); break;
			case k_esc: esc(&build_ready); break;
			case k_quit: outro(); break;
			case k_test: test_storm(); break;
			case k_b:
				build_mode_exchange(&b_barracks_info);
				display_build_list(&build_ready);
				break;
			case k_p: 
				build_mode_exchange(&b_plate_info); 
				patrol_move_exchange();
				break;
			case k_d: build_mode_exchange(&b_dormitory_info); break;
			case k_g: build_mode_exchange(&b_garage_info); break;
			case k_s: 
				build_mode_exchange(&b_shelter_info); 
				make_unit(&b_soldier_info);
				spice_save();
				break;
			case k_m: move_mode_exchange(); break;
			case k_f: make_unit(&b_fremen_info); break;
			case k_1: test_cmd(1); break;
			case k_2: test_cmd(2); break; 
			case k_3: test_cmd(3); break;
			case k_5: test_cmd(5); break;
			case k_6: test_cmd(6); break;
			case k_cmd: test_cmd(0); break;
			case k_none:
			case k_undef:
			default: break;
			}
		}
		ai_engine(); // ai 판단

		unit_search(); // 유닛 탐색
		objects_move(); // 유닛, 샌드웜, 모래폭풍 이동
		units_action(); // 유닛 행동(전투, 채집)	

		display_time();
		display_map();
		display_resource(&resource_change);

		Sleep(TICK);
		sys_clock += 10;
		if (vist_timer != -1) vist_timer += 10;
	}
	return 0;
}

//test
void test_storm() {
	if (storm_test_on) {
		display_system_message("모래폭풍 파괴 테스트 off");
		storm_test_on = 0;
	}
	else {
		display_system_message("모래폭풍 파괴 테스트 on");
		storm_test_on = 1;
	}

}
void make_fighter() {
	if (!f_ready) {
		f_ready = 1;
		display_system_message("위치 선택");
		return;
	}
	unit_push(&r_fighter_info, cursor);
	f_ready = 0;
}
// 인트로 & 종료
void intro() {
	gotoxy((POSITION) { 8, 55 });
	printf("DUNE 1.5");
	gotoxy((POSITION) { 12, 35 });
	printf("출력에 이상이 있을땐 r키를 누르면 화면이 재출력됩니다.");
	gotoxy((POSITION) { 15, 30 });
	printf("상태창이 부족할 시 맵크기를 늘리고 전체화면으로 설정해 주세요.");
	gotoxy((POSITION) { 18, 48 });
	printf("더블 클릭시 이동 거리 : ");
	scanf_s("%d", &double_click_distance);
	system("cls");
}
void outro() {
	char e;
	do {
		display_system_message("종료하시겠습니까? (Y / N) : ");
		bool poo = getchar();
		scanf_s("%c", &e, 1);
	} while (e != 'Y' && e != 'y' && e != 'n' && e != 'N');

	if (e == 'Y' || e == 'y') {
		display_system_message("게임을 종료합니다...");
		Sleep(1000);
		exit(0);
	}
	else {
		display_system_message("게임을 재개합니다.");
	}
}

// 유닛 & 빌딩을 연결리스트에 추가
void unit_push(UNIT_INFO* info, POSITION pos) {
	units[unit_unused].exist = 1;
	units[unit_unused].pos = pos;
	units[unit_unused].dest = pos;
	strncpy_s(units[unit_unused].mode, 15, "wait", 15);
	units[unit_unused].next_move_time = INT_MAX;
	units[unit_unused].hp = info->max_hp;
	units[unit_unused].info_p = info;

	// 비어있는 칸
	int idx = 0;
	while (units[idx].exist) {
		idx++;
	}

	units[unit_unused].next = idx;
	units[idx].pre = unit_unused;
	unit_unused = idx;

	// 
	map_change[pos.x][pos.y] = 1;
}
void unit_erase(POSITION pos) { // 해당 위치의 유닛 삭제
	int idx = 0;
	while (units[idx].exist) {
		if (units[idx].pos.x == pos.x && units[idx].pos.y == pos.y) break;
		idx = units[idx].next;
	}
	if (idx == 0 || !units[idx].exist) return;
	if (units[idx].info_p->color == COLOR_BLUE) resource.population -= units[idx].info_p->population;

	int pre = units[idx].pre;
	int next = units[idx].next;

	units[idx].exist = 0;
	units[pre].next = next;
	units[next].pre = pre;

	map_change[pos.x][pos.y] = 1;
}

void building_push(BUILDING_INFO* info, POSITION pos) {
	buildings[building_unused].exist = 1;
	buildings[building_unused].pos = pos;
	buildings[building_unused].hp = info->max_hp;
	buildings[building_unused].info_p = info;

	// 비어있는 칸
	int idx = 0;
	while (buildings[idx].exist) {
		idx++;
	}

	buildings[building_unused].next = idx;
	buildings[idx].pre = building_unused;
	building_unused = idx;

	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			map_change[pos.x + r][pos.y + c] = 1;
		}
	}
}
void building_erase(POSITION pos) { // 해당 위치의 유닛 삭제 
	int idx = 0;
	while (buildings[idx].exist) {
		if (buildings[idx].pos.x == pos.x && buildings[idx].pos.y == pos.y) break;
		idx = buildings[idx].next;
	}
	if (idx == 0 || !buildings[idx].exist) return;

	int pre = buildings[idx].pre;
	int next = buildings[idx].next;

	buildings[idx].exist = 0;
	buildings[pre].next = next;
	buildings[next].pre = pre;

	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			map_change[pos.x + r][pos.y + c] = 1;
		}
	}
}

// init
inline void init_rock(POSITION pos) {
	map[pos.x][pos.y] = &rock_info;
}
inline void init_4rock(POSITION pos) {
	for (int i = pos.x; i < pos.x + 2; i++) {
		for (int j = pos.y; j < pos.y + 2; j++) {
			POSITION pos = { i, j };
			init_rock(pos);
		}
	}
}
inline void init_total_rock() {
	POSITION pos[5] = { {5,16}, {MAP_HEIGHT - 7, MAP_WIDTH - 18},  { MAP_HEIGHT - 7,10 }, {6, MAP_WIDTH - 11}, {MAP_HEIGHT / 2, MAP_WIDTH / 2} };
	init_4rock(pos[0]);
	init_4rock(pos[1]);
	init_rock(pos[2]);
	init_rock(pos[3]);
	init_rock(pos[4]);
}
void init() {
	units[0].exist = 1;
	units[0].next = 1;
	units[0].info_p = &error_n;
	units[1].pre = 0;
	buildings[0].exist = 1;
	buildings[0].next = 1;
	buildings[0].info_p = &error_b;
	buildings[1].pre = 0;

	// init unit
	unit_push(&b_havester_info, (POSITION) { MAP_HEIGHT - 3, 0 });
	unit_push(&r_havester_info, (POSITION) { 2, MAP_WIDTH - 1 });
	// init building
	building_push(&b_base_info, (POSITION) { MAP_HEIGHT - 2, 0 });
	building_push(&r_base_info, (POSITION) { 0, MAP_WIDTH - 2 });
	//building_push(&b_plate_info, (POSITION) { MAP_HEIGHT - 2, 2 });
	//building_push(&r_plate_info, (POSITION) { 0, MAP_WIDTH - 4 });
	building_push(&spice_info, (POSITION) { MAP_HEIGHT - 5, 0 });
	building_push(&spice_info, (POSITION) { 4, MAP_WIDTH - 1 });

	// plate
	// base
	plate[MAP_HEIGHT - 2][0] = 'B'; 
	plate[MAP_HEIGHT - 2][1] = 'B'; 
	plate[MAP_HEIGHT - 1][0] = 'B'; 
	plate[MAP_HEIGHT - 1][1] = 'B';

	plate[MAP_HEIGHT - 2][2] = 'B';
	plate[MAP_HEIGHT - 2][3] = 'B';
	plate[MAP_HEIGHT - 1][2] = 'B';
	plate[MAP_HEIGHT - 1][3] = 'B';

	// base
	plate[0][MAP_WIDTH - 2] = 'R'; 
	plate[0][MAP_WIDTH - 1] = 'R'; 
	plate[1][MAP_WIDTH - 2] = 'R'; 
	plate[1][MAP_WIDTH - 1] = 'R';

	plate[0][MAP_WIDTH - 3] = 'R';
	plate[0][MAP_WIDTH - 4] = 'R';
	plate[1][MAP_WIDTH - 3] = 'R';
	plate[1][MAP_WIDTH - 4] = 'R';



	sandworm[0].pos = (POSITION){ 0,0 };
	sandworm[1].pos = (POSITION){ MAP_HEIGHT - 1, MAP_WIDTH - 1 };
	for (int i = 0; i < 2; i++) {
		sandworm[i].hp = sandworm_info.max_hp;
		sandworm[i].len = 1;
		sandworm[i].info_p = &sandworm_info;
		sandworm[i].next_move_time = sandworm_info.move_period;
		sandworm[i].next_action_time = sandworm_info.action_period;
		sandworm[i].next_emission_time = 20000;
	}


	for (int r = 0; r < MAP_HEIGHT; r++) {
		for (int c = 0; c < MAP_WIDTH; c++) {
			map[r][c] = &desert_info;
		}
	}
	init_total_rock();

	// cursor

	// selected
	selected_unit = &units[0];
	selected_building = &buildings[0];


	for (int r = 0; r < MAP_HEIGHT; r++) {
		for (int c = 0; c < MAP_WIDTH; c++) {
			map_change[r][c] = 1;
		}
	}
}

// 커서 이동
void cursor_move(DIRECTION dir, int n) { // 방향, 움직일 칸수
	vist_timer = -1;
	if (n == 0) {
		return;
	}


	POSITION new_pos = pmove(cursor, dir);
	// validation check
	if (build_mode) {
		if (new_pos.x < 0 || new_pos.x >= MAP_HEIGHT - 1 || new_pos.y < 0 || new_pos.y >= MAP_WIDTH - 1) return;
		for (int r = 0; r < 2; r++) {
			for (int c = 0; c < 2; c++) {
				map_change[cursor.x + r][cursor.y + c] = 1;
			}
		}
		cursor = new_pos;
		for (int r = 0; r < 2; r++) {
			for (int c = 0; c < 2; c++) {
				map_change[cursor.x + r][cursor.y + c] = 1;
			}
		}
		cursor_move(dir, n - 1);
	}
	else {
		if (new_pos.x < 0 || new_pos.x >= MAP_HEIGHT || new_pos.y < 0 || new_pos.y >= MAP_WIDTH) return;

		map_change[cursor.x][cursor.y] = 1;
		cursor = new_pos;
		map_change[cursor.x][cursor.y] = 1;

		cursor_move(dir, n - 1);
	}
}

// 선택
char get_repr(POSITION pos) { // 해당 위치에 있는 객체의 문자를 가져오는 함수. 
	int idx = get_unit_idx(pos);
	if (idx) {
		return units[idx].info_p->repr;
	}
	idx = get_sandworm_idx(pos);
	if (idx != 3) {
		return sandworm[idx].info_p->repr;
	}
	idx = get_building_idx(pos);
	if (idx) {
		return buildings[idx].info_p->repr;
	}
	return map[pos.x][pos.y]->repr;
}
void select_object() { // 스페이스바 입력시, 해당 위치와 해당 객체의 문자를 저장, 상태창, 명령창 출력. 
	// 건설모드, 하베스터 수확, 이동위치 등 위치를 지정하는 경우에는 선택을 하지 않음.
	if (build_mode || strncmp(selected_unit->mode, "select_s", 10) == 0 || \
		strncmp(selected_unit->mode, "select_p", 10) == 0) return;

	selected_unit = &units[0];
	selected_building = &buildings[0];

	int idx = get_unit_idx(cursor);
	if (idx) {
		selected_unit = &units[idx];
		return;
	}
	idx = get_building_idx(cursor);
	if (idx) {
		selected_building = &buildings[idx];
		return;
	}
	if (plate[cursor.x][cursor.y]) {
		selected_building = &plate_st;
	}
}

// 유닛 공격
void attack_unit(UNIT_INFO* attacker, UNIT* victim) {
	(*victim).hp = ((*victim).hp - attacker->damage <= 0) ? 0 : (*victim).hp - attacker->damage;

	char buff[100];
	snprintf(buff, 100, "%s가 %s에게 공격당했습니다. (남은체력 : %d)", (*victim).info_p->name, attacker->name, (*victim).hp);
	display_system_message(buff);
	if ((*victim).hp == 0) {
		unit_erase((*victim).pos);
		snprintf(buff, 100, "%s가 한줌의 모래로 돌아갔습니다.", (*victim).info_p->name);
		display_system_message(buff);
	}
}

// 샌드웜 관련 함수
bool set_sandworm_dest(SANDWORM* sandworm) { // 너비우선탐색으로 가장 가까운 유닛을 탐색하고, 목적지로 설정. 
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			dist[i][j] = -1;
		}
	}

	int head = 0, tail = 0;
	Q[tail++] = (*sandworm).pos;
	dist[(*sandworm).pos.x][(*sandworm).pos.y] = 0;

	while (head != tail) {
		POSITION pos = Q[head++];

		for (int i = 1; i <= 4; i++) {
			POSITION npos = pmove(pos, i);

			if (npos.x < 0 || npos.x >= MAP_HEIGHT || npos.y < 0 || npos.y >= MAP_WIDTH || dist[npos.x][npos.y] != -1) continue;
			if (get_building_idx(npos) || map[npos.x][npos.y]->repr == 'R' || plate[npos.x][npos.y]) continue;
			if (get_unit_idx(npos)) {
				(*sandworm).dest = npos;
				return 1;
			}

			dist[npos.x][npos.y] = dist[pos.x][pos.y] + 1;
			Q[tail++] = npos;
		}
	}
	return 0;
}
int find_min_dist_s(POSITION pos, POSITION dest) { // 너비우선탐색으로 목적지까지의 거리를 구하는 함수.
	if (pos.x == dest.x && pos.y == dest.y) return 0;

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			dist[i][j] = -1;
		}
	}

	int head = 0, tail = 0;
	Q[tail++] = pos;
	dist[pos.x][pos.y] = 0;

	while (head != tail) {
		POSITION pos = Q[head++];
		for (int i = 1; i <= 4; i++) {
			POSITION npos = pmove(pos, i);

			if (npos.x < 0 || npos.x >= MAP_HEIGHT || npos.y < 0 || npos.y >= MAP_WIDTH || dist[npos.x][npos.y] != -1) continue;
			if (map[npos.x][npos.y]->repr == 'R' || get_sandworm_idx(npos) != 3 || \
				get_building_idx(npos) || plate[npos.x][npos.y]) continue; 
			if (npos.x == dest.x && npos.y == dest.y) {
				return dist[pos.x][pos.y] + 1;
			}

			dist[npos.x][npos.y] = dist[pos.x][pos.y] + 1;
			Q[tail++] = npos;
		}
	}
	return INT_MAX;
}
DIRECTION get_min_dir_s(POSITION pos, POSITION dest) { // 현재 위치에서 상하좌우 네방향중 어느방향으로 가는게 가장 최단거리인지 구함. 
	int min_dist = INT_MAX;
	DIRECTION dir = 0;

	for (DIRECTION d = 1; d <= 4; d++) {
		POSITION next_pos = pmove(pos, d);
		int nx = next_pos.x;
		int ny = next_pos.y;
		if (nx < 0 || nx >= MAP_HEIGHT || ny < 0 || ny >= MAP_WIDTH) continue;
		if (map[nx][ny]->repr == 'R' || get_sandworm_idx(next_pos) != 3 || \
			get_building_idx(next_pos))continue;

		int tmp = find_min_dist_s(next_pos, dest); // 목적지까지의 거리를 반환 
		if (tmp < min_dist) { // 가장 짧은 거리를 저장
			min_dist = tmp;
			dir = d;
		}
	}

	if (min_dist == -1)
		return d_stay;
	else
		return dir;
}
void sandworm_move(SANDWORM* sandworm) {
	if (sys_clock < (*sandworm).next_move_time) return;
	if (!sand_worm_on) return;
	(*sandworm).next_move_time = sys_clock + (*sandworm).info_p->move_period;

	// 목적지 설정
	unit_exist = set_sandworm_dest(sandworm);

	if (!unit_exist && (*sandworm).pos.x == (*sandworm).dest.x && (*sandworm).pos.y == (*sandworm).dest.y) {
		int nx, ny;
		do {
			nx = rand() % (MAP_HEIGHT - 2) + 1;
			ny = rand() % (MAP_WIDTH - 2) + 1;
		} while (map[nx][ny]->repr == 'R' || get_sandworm_idx((POSITION) { nx, ny }) != 3 || \
			get_building_idx((POSITION) { nx, ny }));

		POSITION new_pos = { nx, ny };
		(*sandworm).dest = new_pos;
	}

	// 방향 설정
	DIRECTION dir = get_min_dir_s((*sandworm).pos, (*sandworm).dest);
	POSITION next_pos = pmove((*sandworm).pos, dir);
	if (unit_exist && next_pos.x == (*sandworm).dest.x && next_pos.y == (*sandworm).dest.y) {
		if (sys_clock >= (*sandworm).next_action_time) {
			(*sandworm).next_action_time = sys_clock + sandworm_info.action_period;
			int idx = get_unit_idx(next_pos);
			attack_unit(&sandworm_info, &units[idx]);
		}
		else { // 유닛 앞에 도착했지만 공격주기가 되지 않았을경우.
			display_system_message("샌드웜이 공격을 준비중입니다!");
		}
	}
	else {
		map_change[(*sandworm).pos.x][(*sandworm).pos.y] = 1;
		(*sandworm).pos = next_pos;
		map_change[(*sandworm).pos.x][(*sandworm).pos.y] = 1;
	}
}
void sandworm_emission(SANDWORM* sandworm) {
	if (sys_clock < (*sandworm).next_emission_time) return;
	(*sandworm).next_emission_time += (rand() % 300 + 100) * 1000;
	building_push(&spice_info, (*sandworm).pos);
	int idx = get_building_idx((*sandworm).pos);
	buildings[idx].hp = rand() % 9 + 1;
	char buff[100];
	snprintf(buff, 100, "샌드웜이 스파이스를 배출했습니다.(매장량 : %d)", buildings[idx].hp);
	display_system_message(buff);
}

// 유닛 생성 함수
POSITION get_position(POSITION building_pos) { // 유닛을 생성할때 생성위치를 반환
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			dist[i][j] = -1;
		}
	}


	int head = 0, tail = 0;
	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			Q[tail++] = padd(building_pos, (POSITION) { r, c });
			dist[r][c] = 0;
		}
	}

	while (head != tail) {
		POSITION cur = Q[head++];
		for (int dir = 1; dir <= 4; dir++) {
			POSITION npos = pmove(cur, dir);

			if (npos.x < 0 || npos.x >= MAP_HEIGHT || npos.y < 0 || npos.y >= MAP_WIDTH || dist[npos.x][npos.y] != -1) continue;
			if (get_unit_idx(npos)) continue;
			if (get_building_idx(npos)) continue;

			return npos;
		}
	}
	return (POSITION) { 0, MAP_HEIGHT - 1 };
}
void make_unit(UNIT_INFO* unit_info) { // 유닛을 올바른 위치에 추가하는 함수x
	if (selected_building->info_p->repr != unit_info->mother || selected_building->info_p->color == COLOR_RED) return;
	if (unit_info->repr == 'S' && selected_building->info_p->color != 20) return; // 보병생산은 색깔까지 검사

	POSITION pos = get_position(selected_building->pos);
	if (pos.x == 0 && pos.y == MAP_HEIGHT - 1) {
		display_system_message("유닛을 생산할 공간이 부족합니다.");
		return;
	}
	if (resource.spice - unit_info->cost < 0) {
		display_system_message("스파이스가 부족합니다.");
		return;
	}
	if (resource.population + unit_info->population > resource.population_max) {
		display_system_message("인구수가 부족합니다.");
		return;
	}

	resource.spice -= unit_info->cost;
	resource.population += unit_info->population;
	resource_change = 1;

	unit_push(unit_info, pos);
	char buff[100];
	snprintf(buff, 100, "%s 생산완료.", unit_info->name);
	display_system_message(buff);
}

// 독수리&모래폭풍
void eagle_move(EAGLE* eagle) {
	if (sys_clock < (*eagle).next_move_time) return;
	(*eagle).next_move_time = sys_clock + (*eagle).move_period;

	POSITION diff = psub((*eagle).dest, (*eagle).pos);
	DIRECTION dir;

	if ((*eagle).pos.x == (*eagle).dest.x && (*eagle).pos.y == (*eagle).dest.y) {
		if ((*eagle).dest.x == MAP_HEIGHT - 1)
			(*eagle).dest.x = 0;
		else
			(*eagle).dest.x++;

		(*eagle).dest.y = MAP_WIDTH - 1 - (*eagle).dest.y;
		dir = d_stay;
	}
	else if (abs(diff.x) >= abs(diff.y)) {
		dir = (diff.x >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.y >= 0) ? d_right : d_left;
	}

	POSITION next_pos = pmove((*eagle).pos, dir);

	map_change[(*eagle).pos.x][(*eagle).pos.y] = 1;
	map_change[next_pos.x][next_pos.y] = 1;
	(*eagle).pos = next_pos;
}
void storm_move() {
	if (!storm.exist && sys_clock < storm.exist_time) { // 생성시간이 되지 않음.
		return;
	}

	if (!storm.exist && sys_clock >= storm.exist_time) { // 생성시간이 됨.
		storm.exist = 1;
		storm.exist_time = sys_clock + 5000; // 생성후 5000ms 동안 존재
		display_system_message("모래폭풍이 발생했습니다!");
		do {
			storm.pos.x = rand() % MAP_HEIGHT;
			storm.pos.y = rand() % (MAP_WIDTH - 16) + 8; // 본진 근처에는 생성되지 않음.
		} while (storm.pos.x < 0 || storm.pos.x >= MAP_HEIGHT - 1 || storm.pos.y < 0 || storm.pos.y >= MAP_WIDTH - 1);

		for (int r = 0; r < 2; r++) { // 맵 출력을 위해 map_change = 1 
			for (int c = 0; c < 2; c++) {
				POSITION pos = padd(storm.pos, (POSITION) { r, c });
				map_change[pos.x][pos.y] = 1;
			}
		}
	}

	if (storm.exist && sys_clock >= storm.exist_time) { // 생성 후, 소멸 시간이 됨.
		storm.exist = 0;
		storm.exist_time += 300000; // 300초 뒤 재생성
		storm.next_move_time += 30000;
		display_system_message("모래폭풍이 소멸했습니다.");

		for (int r = 0; r < 2; r++) { // 맵 출력을 위해 map_change = 1 
			for (int c = 0; c < 2; c++) {
				POSITION pos = padd(storm.pos, (POSITION) { r, c });
				map_change[pos.x][pos.y] = 1;
			}
		}
		return;
	}

	// 모래폭풍 이동
	if (sys_clock < storm.next_move_time) return;
	storm.next_move_time += 100;

	DIRECTION dir;
	POSITION n_pos;
	do {
		dir = rand() % 4 + 1;
		n_pos = pmove(storm.pos, dir);
	} while (n_pos.x < 0 || n_pos.x >= MAP_HEIGHT - 1 || n_pos.y < 0 || n_pos.y >= MAP_WIDTH - 1);


	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			POSITION pos = padd(storm.pos, (POSITION) { r, c });
			map_change[pos.x][pos.y] = 1;
		}
	}

	storm.pos = n_pos;

	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			POSITION pos = padd(storm.pos, (POSITION) { r, c });
			map_change[pos.x][pos.y] = 1;
		}
	}
}
void storm_move_test() {
	if (!storm.exist && sys_clock < storm.exist_time) { // 생성시간이 되지 않음.
		return;
	}

	if (!storm.exist && sys_clock >= storm.exist_time) { // 생성시간이 됨.
		storm.exist = 1;
		storm.exist_time = sys_clock + 5000; // 생성후 5000ms 동안 존재
		display_system_message("모래폭풍이 발생했습니다!");

		// 본진에 생성(파괴 테스트용)
		storm.pos.x = MAP_HEIGHT - 3;
		storm.pos.y = 0;


		for (int r = 0; r < 2; r++) { // 맵 출력을 위해 map_change = 1 
			for (int c = 0; c < 2; c++) {
				POSITION pos = padd(storm.pos, (POSITION) { r, c });
				map_change[pos.x][pos.y] = 1;
			}
		}
		return;
	}

	if (storm.exist && sys_clock >= storm.exist_time) { // 생성 후, 소멸 시간이 됨.
		storm.exist = 0;
		storm.exist_time += 30000; // 30초 뒤 재생성
		storm.next_move_time += 30000;
		display_system_message("모래폭풍이 소멸했습니다.");

		for (int r = 0; r < 2; r++) { // 맵 출력을 위해 map_change = 1 
			for (int c = 0; c < 2; c++) {
				POSITION pos = padd(storm.pos, (POSITION) { r, c });
				map_change[pos.x][pos.y] = 1;
			}
		}
		return;
	}

	// 모래폭풍 이동
	if (sys_clock < storm.next_move_time) return;
	storm.next_move_time += 100;

	DIRECTION dir;
	POSITION n_pos;
	do {
		dir = rand() % 4 + 1;
		n_pos = pmove(storm.pos, dir);
	} while (n_pos.x < 0 || n_pos.x >= MAP_HEIGHT - 1 || n_pos.y < 0 || n_pos.y >= MAP_WIDTH - 1);


	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			POSITION pos = padd(storm.pos, (POSITION) { r, c });
			map_change[pos.x][pos.y] = 1;
		}
	}

	storm.pos = n_pos;

	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			POSITION pos = padd(storm.pos, (POSITION) { r, c });
			map_change[pos.x][pos.y] = 1;
		}
	}
}
void storm_action() {
	if (!storm.exist) return;

	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			POSITION pos = padd(storm.pos, (POSITION) { r, c });
			int idx = get_unit_idx(pos);
			if (idx) {
				char buff[100];
				snprintf(buff, 100, "%s이(가) 모래폭풍에 휩쓸렸습니다.", units[idx].info_p->name);
				display_system_message(buff);
				unit_erase(pos);
			}
		}
	}

	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			POSITION pos = padd(storm.pos, (POSITION) { r, c });
			int idx = get_building_idx(pos);
			if (idx && buildings[idx].info_p->repr != 'P' && buildings[idx].info_p->repr != 's' && !buildings[idx].destroied) {
				char buff[100];
				snprintf(buff, 100, "%s이(가) 반파되었습니다.", buildings[idx].info_p->name);
				display_system_message(buff);
				buildings[idx].destroied = 1;
				buildings[idx].hp /= 2;
			}
		}
	}
}

// build
void build_mode_exchange(BUILDING_INFO* info) {
	if (!build_ready) return; // B 입력 X
	build_info = info;
	build_mode = 1;

	// 커서 위치 조정
	if (cursor.x == MAP_HEIGHT - 1) {
		cursor.x -= 1;
	}
	if (cursor.y == MAP_WIDTH - 1) {
		cursor.y -= 1;
	}

	// 커서 출력
	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			map_change[cursor.x + r][cursor.y + c] = 1;
		}
	}


	char buff[100];
	snprintf(buff, 100, "[%s] 건설 준비중.", info->name);
	display_system_message(buff);


	POSITION state_pos = { 2,  MAP_WIDTH + 5 };
	for (int r = 0; r < STA_HEIGHT - 2; r++) {
		erase_message(padd(state_pos, (POSITION) { 2 + r, 0 }), STA_WIDTH - 1);
	}

	print_message(padd(state_pos, (POSITION) { 2, 0 }), "[건설모드]");

	POSITION pos = { 4, 0 };
	for (int r = 0; r < info->state_message.about_size; r++) {
		print_message(padd(state_pos, pos), info->state_message.message[r]);
		pos.x += 2;
	}
	for (int r = info->state_message.about_size; r < info->state_message.size; r++) {
		print_message(padd(state_pos, pos), info->state_message.message[r]);
		pos.x += 1;
	}
}
void build() {
	if (!build_mode) return;

	if (resource.spice < build_info->cost) {
		display_system_message("스파이스가 부족합니다.");
		return;
	}

	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 2; c++) {
			POSITION pos = padd(cursor, (POSITION) { r, c });
			if (get_unit_idx(pos) || get_sandworm_idx(pos) != 3) {
				display_system_message("범위내에 유닛이 있습니다.");
				return;
			}
			if (build_info->repr != 'P') { 
				if (build_info->color == COLOR_BLUE && plate[pos.x][pos.y] != 'B') {
					display_system_message("장판이 필요합니다.");
					return;
				}
				if (build_info->color == COLOR_RED && plate[pos.x][pos.y] != 'R') {
					display_system_message("장판이 필요합니다.");
					return;
				}
			}
			if (get_building_idx(pos)) {
				display_system_message("범위내에 구조물이 있습니다.");
				return;
			}
			if (map[pos.x][pos.y]->repr == 'R') {
				display_system_message("범위내에 구조물이 있습니다.");
				return;
			}
		}
	}

	if (build_info->repr == 'P') { // 장판의 경우
		char color = (build_info->color == COLOR_BLUE) ? 'B' : 'R';
		plate[cursor.x][cursor.y] = color;  
		plate[cursor.x][cursor.y + 1] = color; 
		plate[cursor.x + 1][cursor.y] = color; 
		plate[cursor.x + 1][cursor.y + 1] = color;  
	}
	else { // 그 외 건물
		building_push(build_info, cursor); 
	}

	resource.spice -= build_info->cost; 
	if (build_info->color == COLOR_BLUE || build_info->color == 20) {
		char buff[100]; 
		snprintf(buff, 100, "[%s] 건설완료.", build_info->name); 
		display_system_message(buff); 
	} 

	// 숙소 인구수 += 10
	if (build_info->repr == 'D') {
		resource.population_max += 10;
	}
	// 창고 스파이스최대량 += 10
	if (build_info->repr == 'G') {
		resource.spice_max += 10;
	}

	resource_change = 1;
	build_mode = 0;
	build_ready = 0;

	esc(&build_ready);
}

// 유닛 이동
int find_min_dist(POSITION pos, POSITION dest) { // 너비우선탐색으로 목적지까지의 거리를 구하는 함수.
	if (pos.x == dest.x && pos.y == dest.y) return 0;

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			dist[i][j] = -1;
		}
	}

	int head = 0, tail = 0;
	Q[tail++] = pos;
	dist[pos.x][pos.y] = 0;

	while (head != tail) {
		POSITION pos = Q[head++];
		for (int i = 1; i <= 4; i++) {
			POSITION npos = pmove(pos, i);

			// 맵을 벗어날 때
			if (npos.x < 0 || npos.x >= MAP_HEIGHT || npos.y < 0 || npos.y >= MAP_WIDTH || dist[npos.x][npos.y] != -1) continue;

			// 목적지 도착
			if (npos.x == dest.x && npos.y == dest.y) {
				return dist[pos.x][pos.y] + 1;
			}

			// 이동위치에 장애물(유닛, 샌드웜, 건물)이 있음
			if (map[npos.x][npos.y]->repr == 'R' || get_sandworm_idx(npos) != 3 || \
				get_building_idx(npos) || get_unit_idx(npos)) continue;


			dist[npos.x][npos.y] = dist[pos.x][pos.y] + 1;
			Q[tail++] = npos;
		}
	}
	return INT_MAX;
}
DIRECTION get_min_dir(POSITION pos, POSITION dest) { // 현재 위치에서 상하좌우 네방향중 어느방향으로 가는게 가장 최단거리인지 구함. 
	int min_dist = INT_MAX;
	DIRECTION dir = 0;

	for (DIRECTION d = 1; d <= 4; d++) {
		POSITION next_pos = pmove(pos, d);
		int nx = next_pos.x;
		int ny = next_pos.y;


		if (nx < 0 || nx >= MAP_HEIGHT || ny < 0 || ny >= MAP_WIDTH) continue;
		if (map[nx][ny]->repr == 'R' || get_sandworm_idx(next_pos) != 3 || \
			get_building_idx(next_pos) || get_unit_idx(next_pos))continue;

		int tmp = find_min_dist(next_pos, dest); // 목적지까지의 거리를 반환 
		if (tmp < min_dist) { // 가장 짧은 거리를 저장
			min_dist = tmp;
			dir = d;
		}
	}

	if (min_dist == -1)
		return d_stay;
	else
		return dir;
}
void move_to_base(UNIT* unit, char mode) {
	if (resource.spice + unit->havest_num > resource.spice_max) {
		resource.spice = resource.spice_max;
		resource_change = 1;
		display_system_message("스파이스가 가득 찼습니다.");
		strncpy_s(unit->mode, 15, "wait", 15);
		return;
	}
	else {
		resource.spice += unit->havest_num;
		resource_change = 1;
	}

	if (mode == 'w') {
		strncpy_s(unit->mode, 15, "wait", 15);
	}
	else {
		unit->dest = unit->point2; 
		strncpy_s(unit->mode, 15, "move_to_s", 15);
		unit->next_move_time = sys_clock + unit->info_p->move_period; // 이동 활성화   
	}
	unit->havest_num = 0;
}
void unit_move(UNIT* unit) {
	if (sys_clock < unit->next_move_time) return;
	unit->next_move_time += unit->info_p->move_period;

	if (strncmp(unit->mode, "move_to_e", 15) == 0) { // 유닛 전투 목적지 설정 
		if (!get_unit_idx(*(unit->combat_pos_p))) { // 이동중 상대 유닛이 사망
			strncpy_s(unit->mode, 15, "wait", 15);
			unit->next_move_time = INT_MAX; // 이동 비활성화
			return;
		}
		unit->dest = *(unit->combat_pos_p);  
	}

	// 이동
	DIRECTION dir = get_min_dir(unit->pos, unit->dest);
	POSITION next_pos = pmove(unit->pos, dir);
	map_change[unit->pos.x][unit->pos.y] = 1; 
	unit->pos = next_pos; 
	map_change[unit->pos.x][unit->pos.y] = 1; 

	// 목적지 도착
	if (unit->dest.x == unit->pos.x && unit->dest.y == unit->pos.y) {
		unit->next_move_time = INT_MAX; // 이동 비활성화

		// 목적지에 도착했을때, 유닛 상태를 확인
		if (strncmp(unit->mode, "move_to_b", 15) == 0) { // 스파이스 -> 본진
			move_to_base(unit, ' ');
		} 
		else if (strncmp(unit->mode, "move_to_b_w", 15) == 0) { // 스파이스 -> 본진대기
			move_to_base(unit, 'w');
		}
		else if (strncmp(unit->mode, "move", 15) == 0) {
			char buff[100];
			snprintf(buff, 100, "[%s]가 목적지에 도착했습니다.", unit->info_p->name);
			display_system_message(buff);
			strncpy_s(unit->mode, 15, "wait", 15);
		}
		else if (strncmp(unit->mode, "patrol_to_1", 15) == 0) { // 순찰중, 포인트2 -> 포인트 1
			unit->next_move_time = sys_clock + unit->info_p->move_period; // 이동 활성화   
			unit->dest = unit->point2;
			strncpy_s(unit->mode, 15, "patrol_to_2", 15);
		}
		else if (strncmp(unit->mode, "patrol_to_2", 15) == 0) { // 순찰중, 포인트1 -> 포인트 2 
			unit->next_move_time = sys_clock + unit->info_p->move_period; // 이동 활성화   
			unit->dest = unit->point1; 
			strncpy_s(unit->mode, 15, "patrol_to_1", 15); 
		}
	}

	// 목적지 인접위치 도착 확인(스파이스, 전투대상, 공격건물)
	if (strncmp(unit->mode, "move_to_e", 15) == 0) { // 인접한 칸에 전투 대상이 있는지 확인
		for (DIRECTION d = 1; d <= 4; d++) { 
			POSITION pos = padd(unit->pos, dtop(d)); 
			if (pos.x == unit->combat_pos_p->x && pos.y == unit->combat_pos_p->y) {
				int idx = get_unit_idx(pos);
				// 이동 비활성화
				unit->next_move_time = INT_MAX; 
				units[idx].next_move_time = INT_MAX; 
				// 행동(전투)활성화
				unit->next_action_time = sys_clock + unit->info_p->action_period;
				units[idx].next_action_time = sys_clock + units[idx].info_p->action_period;
				// 모드 변경
				strncpy_s(unit->mode, 15, "combat", 15);
				strncpy_s(units[idx].mode, 15, "combat", 15); 
				// 전투상대 좌표 다시한번 저장(시야가 1인경우 지정되지 않을 수 있음)
				units[idx].combat_pos_p = &unit->pos;
				unit->combat_pos_p = &units[idx].pos;
				// 시스템 메시지
				char buff[100];
				snprintf(buff, 100, "[%s] vs [%s] 전투 시작.", unit->info_p->name, units[idx].info_p->name);
				display_system_message(buff);
			}
		}
	}
	if (strncmp(unit->mode, "move_to_bd", 15) == 0) { // 인접한 칸에 공격 건물이 있는지 확인
		int idx = get_building_idx(unit->dest); 
		for (DIRECTION d = 1; d <= 4; d++) {  
			POSITION pos = padd(unit->pos, dtop(d));  
			int n_idx = get_building_idx(pos); 
			if (n_idx == idx) {
				strncpy_s(unit->mode, 15, "attack_b", 15); 
				unit->next_action_time = sys_clock + unit->info_p->action_period; 
				unit->next_move_time = INT_MAX;
			}
		}
	}
	if (strncmp(unit->mode, "move_to_s", 15) == 0) {
		int idx = get_building_idx(unit->dest);  
		for (DIRECTION d = 1; d <= 4; d++) { 
			POSITION pos = padd(unit->pos, dtop(d)); 
			int n_idx = get_building_idx(pos);
			if (n_idx == idx) {
				unit->next_action_time = sys_clock + unit->info_p->action_period; // 행동(수확)활성화 
				unit->next_move_time = INT_MAX;
				strncpy_s(unit->mode, 15, "wait_h", 15); // '수확 대기' 모드  
				// display_system_message("하베스터가 수확중입니다."); 
			}
		}
	}
	

	// 하베스터가 집으로 돌아갈때 목적지를 실시간으로 설정
	if (strncmp(unit->mode, "move_to_b", 10) == 0 || \
		strncmp(unit->mode, "move_to_b_w", 10) == 0) {
		unit->dest = get_position((POSITION) { MAP_HEIGHT - 2, 0 });
	}
}
void units_move() {
	int idx = 0;
	while (units[units[idx].next].exist) { // 다음 유닛이 없을때 까지 선회
		idx = units[idx].next;
		unit_move(&units[idx]);

	}
}

// 유닛 행동 
void haveste(UNIT *unit) {
	unit->next_move_time = sys_clock + unit->info_p->move_period / 5; // 이동 활성화 
	unit->dest = get_position((POSITION) { MAP_HEIGHT - 2, 0 });  // 목적지 설정

	for (DIRECTION dir = 1; dir <= 4; dir++) {
		POSITION pos = padd(unit->pos, dtop(dir)); 
		int idx = get_building_idx(pos);
		if (buildings[idx].info_p->repr == 's') { 
			unit->havest_num = rand() % 2 + 2; 
			if (buildings[idx].hp - unit->havest_num <= 0) { 
				unit->havest_num = buildings[idx].hp; 
				buildings[idx].hp = 0; 
				building_erase(pos); 
				display_system_message("해당 스파이스가 바닥났습니다!"); 
				strncpy_s(unit->mode, 15, "move_to_b_w", 15); // 베이스로 이동후 대기하라는 의미 
			} 
			else {
				buildings[idx].hp -= unit->havest_num; 
				strncpy_s(unit->mode, 15, "move_to_b", 15); // 베이스로 이동중이라는 의미  
			}

			//char buff[100]; 
			//snprintf(buff, 100, "하베스터가 스파이스를 채집했습니다. (채집량 %d)", unit->havest_num); 
			//display_system_message(buff); 

			map_change[pos.x][pos.y] = 1; 
			return; 
		}
	}
	// 스파이스가 사라졌을때
	strncpy_s(unit->mode, 15, "move_to_b_w", 15); // 베이스로 이동중이라는 의미 
}
void combat(UNIT *unit) { 
	unit->next_action_time += unit->info_p->action_period;

	int idx = get_unit_idx(*(unit->combat_pos_p)); // 전투 상대 유닛
	if (units[idx].hp - unit->info_p->damage > 0) {
		units[idx].hp -= unit->info_p->damage;

		char buff[100];
		snprintf(buff, 100, "[%s]가 공격당했습니다.(남은체력 : %d)", units[idx].info_p->name, units[idx].hp);
		display_system_message(buff); 
	}
	else {
		unit_erase(units[idx].pos);
		strncpy_s(unit->mode, 15, "wait", 15);
		unit->next_action_time = INT_MAX;
		char buff[100]; 
		snprintf(buff, 100, "[%s]가 사망했습니다.", units[idx].info_p->name); 
		display_system_message(buff); 
	}
}
void attack_building(UNIT *unit) {
	unit->next_action_time += unit->info_p->action_period;

	int idx = get_building_idx(unit->dest);
	if (buildings[idx].hp - unit->info_p->damage > 0) {
		buildings[idx].hp -= unit->info_p->damage; 

		char buff[100];
		snprintf(buff, 100, "[%s]이(가) 공격당했습니다.(남은체력 : %d)", buildings[idx].info_p->name, buildings[idx].hp);
		display_system_message(buff);
	}
	else {
		building_erase(buildings[idx].pos);
		strncpy_s(unit->mode, 15, "wait", 15);
		unit->next_action_time = INT_MAX; 
		char buff[100]; 
		snprintf(buff, 100, "[%s]이(가) 파괴되었습니다.", buildings[idx].info_p->name); 
		display_system_message(buff); 
	}

}
void unit_action(UNIT* unit) {
	if (sys_clock < unit->next_action_time) return;

	if (strncmp(unit->mode, "wait_h", 15) == 0) {
		if (unit->info_p->color == COLOR_BLUE) {
			haveste(unit); 
		}
		else {

		}
	}
	else if (strncmp(unit->mode, "combat", 15) == 0) {
		combat(unit);
	}
	else if (strncmp(unit->mode, "attack_b", 15) == 0) {
		attack_building(unit);
	}
}
void units_action() {
	int idx = 0;
	while (units[units[idx].next].exist) { // 다음 유닛이 없을때 까지 순회 
		idx = units[idx].next;
		unit_action(&units[idx]);
	}
}
void patrol_move_exchange() {
	if (selected_unit->info_p->color == COLOR_RED) return; 

	display_system_message("순찰할 위치를 선택해 주세요.");
	strncpy_s(selected_unit->mode, 15, "select_pp", 15);
}

// 하베스터
void harvest_mode_exchange() {
	// 선택 유닛이 블루팀 하베스터가 아닐때
	if (selected_unit->info_p->repr != 'H' || selected_unit->info_p->color == COLOR_RED) return;

	display_system_message("수확할 스파이스를 선택해 주세요.");
	strncpy_s(selected_unit->mode, 15, "select_s", 15);
}
void set_havest_dest() {
	if (strncmp(selected_unit->mode, "select_s", 15) != 0) return;
	int idx = get_building_idx(cursor);
	if (buildings[idx].info_p->repr != 's') {
		display_system_message("스파이스가 아닙니다.");
		display_system_message("다시 선택해 주세요.");
		return;
	}

	selected_unit->point2 = cursor; 
	selected_unit->dest = cursor;
	selected_unit->next_move_time = sys_clock + selected_unit->info_p->move_period;

	strncpy_s(selected_unit->mode, 15, "move_to_s", 15); // 스파이스로 이동중이라는 의미
	display_system_message("해당 스파이스로 이동합니다.");
	selected_unit = &units[0];
	return;
}
// 수동 저장
void spice_save() { // 하베스터가 스파이스를 들고있는 상태에서, 다른곳으로 이동하다 베이스에 도착했을때 가지고있던 스파이스를 저장할수 있는 기능.

	// 선택 유닛이 블루팀 하베스터가 아닐때
	if (selected_unit->info_p->repr != 'H' || selected_unit->info_p->color == COLOR_RED) return; 

	// 하베스터가 베이스와 인접해있는지 확인
	for (DIRECTION dir = 1; dir <= 4; dir++) {
		POSITION pos = padd(cursor, dtop(dir));
		
		if (buildings[get_building_idx(pos)].info_p->repr == 'B') { 
			if (selected_unit->havest_num) {
				if (resource.spice + selected_unit->havest_num < resource.spice_max) {
					resource.spice += selected_unit->havest_num;
					display_system_message("스파이스를 저장했습니다.");
				}
				else {
					resource.spice = resource.spice_max;
					selected_unit->havest_num = resource.spice_max - resource.spice + selected_unit->havest_num;
				}
				resource_change = 1;
				return;
			}
			display_system_message("보유중인 스파이스가 없습니다.");
			return;
		}
	}
	display_system_message("본진과 인접한 곳에서 시도해주세요.");
}

// 이동
void move_mode_exchange() {
	if (selected_unit->info_p->color != COLOR_BLUE) return;

	display_system_message("이동할 위치를 선택해 주세요.");
	strncpy_s(selected_unit->mode, 15, "select_p", 15);
}
void set_selected_unit_dest() { // 선택된 유닛의 목적지를 설정하는 함수 
	if (strncmp(selected_unit->mode, "select_p", 15) == 0) { // 이동 목적지
		int idx = get_building_idx(cursor); 
		if (buildings[idx].info_p->color == COLOR_RED) { // 건물 선택
			selected_unit->dest = buildings[idx].pos; 
			display_system_message("이동을 시작합니다.");
			strncpy_s(selected_unit->mode, 15, "move_to_bd", 15);
			selected_unit->next_move_time = sys_clock + selected_unit->info_p->move_period;
			return;
		}
		if (!get_unit_idx(cursor) && get_sandworm_idx(cursor) == 3) {  
			display_system_message("이동을 시작합니다.");
			strncpy_s(selected_unit->mode, 15, "move", 15);
			selected_unit->dest = cursor;
			selected_unit->next_move_time = sys_clock + selected_unit->info_p->move_period;
			return;
		}
		display_system_message("유닛&건물이 없는 위치를 선택해 주세요"); 
		display_system_message("다시 선택해 주세요"); 
	}
	else if (strncmp(selected_unit->mode, "select_pp", 15) == 0) { // 순찰 목적지
		if (!get_unit_idx(cursor) && !get_building_idx(cursor) && get_sandworm_idx(cursor) == 3) {
			display_system_message("순찰을 시작합니다.");
			strncpy_s(selected_unit->mode, 15, "patrol_to_2", 15); // "포인트 2로 향하는 중"의미
			selected_unit->point1 = selected_unit->pos; // 포인트 1 : 현재위치
			selected_unit->point2 = cursor;  // 포인트 2 : 지정위치
			selected_unit->dest = cursor; 
			selected_unit->next_move_time = sys_clock + selected_unit->info_p->move_period; 
			return;
		}
		display_system_message("유닛&건물이 없는 위치를 선택해 주세요"); 
		display_system_message("다시 선택해 주세요"); 
	}
}

// 전투
void combat_mode_exchange(UNIT *unit) {
	if (strncmp(unit->mode, "move_to_e", 15) == 0 || strncmp(unit->mode, "combat", 15) == 0) return;

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			dist[i][j] = -1;
		}
	}
	 
	int head = 0, tail = 0;
	Q[tail++] = unit->pos;
	dist[unit->pos.x][unit->pos.y] = 0; 

	while (head != tail) {
		POSITION pos = Q[head++];
		if (dist[pos.x][pos.y] == unit->info_p->sight) return;

		for (DIRECTION dir = 1; dir <= 4; dir++) {
			POSITION npos = pmove(pos, dir);

			// 맵을 벗어날 때
			if (npos.x < 0 || npos.x >= MAP_HEIGHT || npos.y < 0 || npos.y >= MAP_WIDTH || dist[npos.x][npos.y] != -1) continue;
			int idx = get_unit_idx(npos);
			if (idx) {
				if (unit->info_p->color == COLOR_BLUE && units[idx].info_p->color == COLOR_RED\
					|| unit->info_p->color == COLOR_RED && units[idx].info_p->color == COLOR_BLUE) { // 블루 -> 레드 , 레드 -> 블루
					strncpy_s(unit->mode, 15, "move_to_e", 15); // 전투를 위해 적에게 이동   
					unit->combat_pos_p = &units[idx].pos;
					unit->next_move_time = sys_clock + unit->info_p->move_period; // 이동 활성화
					return;
				}
			}

			dist[npos.x][npos.y] = dist[pos.x][pos.y] + 1;
			Q[tail++] = npos; 
		}
	}
}
void unit_search() { // 시야에 상대 유닛이 보이는지 확인
	int idx = 0; 
	while (units[units[idx].next].exist) { // 다음 유닛이 없을때 까지 순회  
		idx = units[idx].next; 
		combat_mode_exchange(&units[idx]);  
	}
}

// inline
inline space_action() {
	set_selected_unit_dest();
	set_havest_dest();
	build();
	select_object();
	display_state_message();
	display_cmd_message();
	make_test_object(); 
}
inline void objects_move() {
	storm_action();
	for (int i = 0; i < 2; i++) {
		sandworm_emission(&sandworm[i]);
		sandworm_move(&sandworm[i]);
	}
	eagle_move(&eagle);
	if (storm_test_on) {
		storm_move_test();
	}
	else {
		storm_move();
	}
	units_move();
}