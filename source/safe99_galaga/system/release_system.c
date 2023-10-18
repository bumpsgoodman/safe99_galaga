//***************************************************************************
// 
// 파일: release_system.c
// 
// 설명: ecs 시스템 해제
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/18
// 
//***************************************************************************

#include "precompiled.h"
#include "game.h"

void release_mesh_system(const ecs_view_t* p_view)
{
    for (size_t i = 0; i < p_view->num_archetypes; ++i)
    {
        i_mesh2_t** pp_mesh = (i_mesh2_t**)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->mesh_component);

        const size_t num_instances = p_view->p_this->vtbl->get_num_instances(p_view, i);
        for (size_t j = 0; j < num_instances; ++j)
        {
            // 메시 해제
            SAFE_RELEASE(pp_mesh[j]);
        }
    }
}