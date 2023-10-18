//***************************************************************************
// 
// 파일: enemy2.h
// 
// 설명: ecs enemy2 컴포넌트
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/18
// 
//***************************************************************************

#ifndef ENEMY2_H
#define ENEMY2_H

#include "safe99_common/defines.h"
#include "safe99_ecs/i_ecs.h"

typedef ecs_component enemy2
{
    float speed;
} enemy2_t;

#endif // ENEMY2_H