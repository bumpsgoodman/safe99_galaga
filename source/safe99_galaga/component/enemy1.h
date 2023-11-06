//***************************************************************************
// 
// 파일: enemy1.h
// 
// 설명: ecs enemy1 컴포넌트
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/18
// 
//***************************************************************************

#ifndef ENEMY1_H
#define ENEMY1_H

#include "safe99_common/defines.h"
#include "safe99_ecs/i_ecs.h"

typedef ecs_component enemy1
{
    float speed;
} enemy1_t;

#endif // ENEMY1_H