//***************************************************************************
// 
// 파일: collide_system.c
// 
// 설명: ecs 충돌 처리 시스템
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/19
// 
//***************************************************************************

#include "precompiled.h"
#include "game.h"

void collide_system(const ecs_view_t* p_view)
{
    for (size_t i = 0; i < p_view->num_archetypes; ++i)
    {
        const i_mesh2_t** pp_mesh = (i_mesh2_t**)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->mesh_component);
        const transform2_t* p_transforms = (transform2_t*)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->transform_component);

        const size_t num_instances = p_view->p_this->vtbl->get_num_instances(p_view, i);
        const ecs_id_t* entities = (ecs_id_t*)gp_game->p_ecs->vtbl->get_entities_or_null(p_view, i);
        for (size_t j = 0; j < num_instances; ++j)
        {
            const transform2_t* p_transform = p_transforms + j;
            const ecs_id_t entity = entities[j];

            // 미사일이 아니면 패스
            if (!gp_game->p_ecs->vtbl->has_component(gp_game->p_ecs, entity, 1, gp_game->missile_component))
            {
                break;
            }
            
            const vector2_t pos = p_transform->position;
            for (size_t ti = 0; ti < p_view->num_archetypes; ++ti)
            {
                const transform2_t* p_target_transforms = (transform2_t*)p_view->p_this->vtbl->get_instances_or_null(p_view, ti, gp_game->transform_component);
                const i_mesh2_t** pp_target_mesh = (i_mesh2_t**)p_view->p_this->vtbl->get_instances_or_null(p_view, ti, gp_game->mesh_component);

                const size_t num_target_instances = p_view->p_this->vtbl->get_num_instances(p_view, ti);
                const ecs_id_t* target_entities = (ecs_id_t*)gp_game->p_ecs->vtbl->get_entities_or_null(p_view, ti);
                for (size_t tj = 0; tj < num_target_instances; ++tj)
                {
                    const ecs_id_t target_entity = target_entities[tj];

                    const bool b_enemy = gp_game->p_ecs->vtbl->has_component(gp_game->p_ecs, target_entity, 1, gp_game->enemy1_component)
                        || gp_game->p_ecs->vtbl->has_component(gp_game->p_ecs, target_entity, 1, gp_game->enemy2_component);

                    // 적이 아니면 패스
                    if (!b_enemy)
                    {
                        break;
                    }

                    const transform2_t* p_target_transform = p_target_transforms + tj;

                    const float half_scale = p_target_transform->scale * 0.5f;
                    const vector2_t left_top = { p_target_transform->position.x - half_scale, p_target_transform->position.y - half_scale };
                    const vector2_t bottom_right = { p_target_transform->position.x + half_scale, p_target_transform->position.y + half_scale };

                    if (pos.x >= left_top.x && pos.x <= bottom_right.x
                        && pos.y >= left_top.y && pos.y <= bottom_right.y)
                    {
                        gp_game->p_ecs->vtbl->destroy_entity(gp_game->p_ecs, entity);
                        gp_game->p_ecs->vtbl->destroy_entity(gp_game->p_ecs, target_entity);
                    }
                }
            }
        }
    }
}