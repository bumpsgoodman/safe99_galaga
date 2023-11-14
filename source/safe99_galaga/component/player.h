//***************************************************************************
// 
// 파일: player.h
// 
// 설명: ecs player 컴포넌트
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/18
// 
//***************************************************************************

#ifndef PLAYER_H
#define PLAYER_H

#include "safe99_common/defines.h"
#include "safe99_ecs/i_ecs.h"

typedef struct player
{
    float speed;
} player_t;

#endif // PLAYER_H