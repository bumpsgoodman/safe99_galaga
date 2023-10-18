//***************************************************************************
// 
// 파일: missile.h
// 
// 설명: ecs 미사일 컴포넌트
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/18
// 
//***************************************************************************

#ifndef MISSILE_H
#define MISSILE_H

typedef ecs_component missile
{
    vector2_t start_pos;
    float max_distance;
    float speed;
} missile_t;

#endif // MISSILE_H