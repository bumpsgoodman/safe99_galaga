//***************************************************************************
// 
// 파일: control_system.h
// 
// 설명: ecs 컨트롤 시스템
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/18
// 
//***************************************************************************

#ifndef CONTROL_SYSTEM
#define CONTROL_SYSTEM

#include "safe99_common/defines.h"
#include "safe99_ecs/i_ecs.h"

void control_player_system(const ecs_view_t* p_view);
void control_missile_system(const ecs_view_t* p_view);

#endif // CONTROL_SYSTEM