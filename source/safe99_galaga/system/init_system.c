//***************************************************************************
// 
// 파일: init_system.c
// 
// 설명: ecs 초기화 시스템
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/18
// 
//***************************************************************************

#include "precompiled.h"
#include "game.h"

void init_player_system(const ecs_view_t* p_view)
{
    for (size_t i = 0; i < p_view->num_archetypes; ++i)
    {
        i_mesh2_t** pp_mesh = (i_mesh2_t**)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->mesh_component);
        transform2_t* p_transforms = (transform2_t*)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->transform_component);
        player_t* p_players = (player_t*)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->player_component);

        const size_t num_instances = p_view->p_this->vtbl->get_num_instances(p_view, i);
        for (size_t j = 0; j < num_instances; ++j)
        {
            transform2_t* p_transform = p_transforms + j;
            player_t* p_player = p_players + j;

            // 메시 등록
            pp_mesh[j] = gp_game->p_player_mesh;

            // 트랜스폼 초기화
            p_transform->rotation = 0.0f;
            p_transform->scale = 32.0f;

            p_transform->position.x = 0.0f;
            p_transform->position.y = 0.0f;

            // 플레이어 초기화
            p_player->speed = 180.0f;
        }
    }
}

void init_enemy1_system(const ecs_view_t* p_view)
{
    for (size_t i = 0; i < p_view->num_archetypes; ++i)
    {
        i_mesh2_t** pp_mesh = (i_mesh2_t**)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->mesh_component);
        transform2_t* p_transforms = (transform2_t*)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->transform_component);
        enemy1_t* p_enemies = (enemy1_t*)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->enemy1_component);

        const size_t num_instances = p_view->p_this->vtbl->get_num_instances(p_view, i);
        for (size_t j = 0; j < num_instances; ++j)
        {
            transform2_t* p_transform = p_transforms + j;
            enemy1_t* p_enemy = p_enemies + j;

            // 메시 등록
            pp_mesh[j] = gp_game->p_enemy1_mesh;

            // 트랜스폼 초기화
            p_transform->rotation = 0.0f;
            p_transform->scale = 32.0f;

            p_transform->position.x = rand() / (float)RAND_MAX * 10000.0f - 5000.0f;
            p_transform->position.y = rand() / (float)RAND_MAX * 10000.0f - 5000.0f;

            // 적 초기화
            p_enemy->speed = 50.0f;
        }
    }
}

void init_enemy2_system(const ecs_view_t* p_view)
{
    for (size_t i = 0; i < p_view->num_archetypes; ++i)
    {
        i_mesh2_t** pp_mesh = (i_mesh2_t**)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->mesh_component);
        transform2_t* p_transforms = (transform2_t*)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->transform_component);
        enemy2_t* p_enemies = (enemy2_t*)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->enemy2_component);

        const size_t num_instances = p_view->p_this->vtbl->get_num_instances(p_view, i);
        for (size_t j = 0; j < num_instances; ++j)
        {
            transform2_t* p_transform = p_transforms + j;
            enemy2_t* p_enemy = p_enemies + j;

            // 메시 등록
            pp_mesh[j] = gp_game->p_enemy2_mesh;

            // 트랜스폼 초기화
            p_transform->rotation = 0.0f;
            p_transform->scale = 32.0f;

            p_transform->position.x = rand() / (float)RAND_MAX * 10000.0f - 5000.0f;
            p_transform->position.y = rand() / (float)RAND_MAX * 10000.0f - 5000.0f;

            // 적 초기화
            p_enemy->speed = 50.0f;
        }
    }
}