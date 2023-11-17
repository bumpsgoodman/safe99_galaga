//***************************************************************************
// 
// 파일: collide_system.h
// 
// 설명: ecs 충돌 처리 시스템
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/19
// 
//***************************************************************************

#ifndef COLLIDE_SYSTEM_H
#define COLLIDE_SYSTEM_H

#include "safe99_common/defines.h"
#include "safe99_geometry/i_ecs.h"

void collide_system(const ecs_view_t* p_view);

#endif // COLLIDE_SYSTEM_H