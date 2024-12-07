#ifndef _AI_H_  
#define _AI_H_ 


void ai_engine();
void r_havest(UNIT* unit); 
void r_move_to_base(UNIT* unit, char mode);
POSITION find_r_base(POSITION pos);

#endif