//***************************************************************************
// 
// 파일: render_system.h
// 
// 설명: ecs 렌더링 시스템
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/18
// 
//***************************************************************************

#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "safe99_common/defines.h"
#include "safe99_ecs/i_ecs.h"

void render_mesh_system(const ecs_view_t* p_view);

#endif // RENDER_SYSTEM_H