//***************************************************************************
// 
// 파일: release_system.h
// 
// 설명: ecs 시스템 해제
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/18
// 
//***************************************************************************

#ifndef RELEASE_SYSTEM_H
#define RELEASE_SYSTEM_H

#include "safe99_common/defines.h"
#include "safe99_ecs/i_ecs.h"

void release_mesh_system(const ecs_view_t* p_view);

#endif // RELEASE_SYSTEM_H