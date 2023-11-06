//***************************************************************************
// 
// 파일: init_system.h
// 
// 설명: ecs 초기화 시스템
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/18
// 
//***************************************************************************

#ifndef INIT_SYSTEM_H
#define INIT_SYSTEM_H

#include "safe99_common/defines.h"
#include "safe99_ecs/i_ecs.h"

void init_player_system(const ecs_view_t* p_view);
void init_enemy1_system(const ecs_view_t* p_view);
void init_enemy2_system(const ecs_view_t* p_view);

#endif // INIT_SYSTEM_H