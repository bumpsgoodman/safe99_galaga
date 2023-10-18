//***************************************************************************
// 
// 파일: control_system.c
// 
// 설명: ecs 컨트롤 시스템
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/18
// 
//***************************************************************************

#include "precompiled.h"
#include "game.h"

void control_player_system(const ecs_view_t* p_view)
{
    const float delta_time = gp_game->p_renderer->vtbl->get_delta_time(gp_game->p_renderer);

    for (size_t i = 0; i < p_view->num_archetypes; ++i)
    {
        transform2_t* p_transforms = (transform2_t*)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->transform_component);
        player_t* p_players = (player_t*)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->player_component);

        const size_t num_instances = p_view->p_this->vtbl->get_num_instances(p_view, i);
        for (size_t j = 0; j < num_instances; ++j)
        {
            transform2_t* p_transform = p_transforms + j;
            player_t* p_player = p_players + j;

            if (get_key_state(VK_LEFT))
            {
                p_transform->rotation -= 180.0f * delta_time;
            }

            if (get_key_state(VK_RIGHT))
            {
                p_transform->rotation += 180.0f * delta_time;
            }

            if (get_key_state(VK_UP))
            {
                const float rad = p_transform->rotation * (float)PI_DIV_180;
                float sin;
                float cos;
                get_sin_cos(rad, &sin, &cos);
                p_transform->position.x += p_player->speed * sin * delta_time;
                p_transform->position.y += p_player->speed * cos * delta_time;
            }

            if (get_key_state(VK_DOWN))
            {
                const float rad = p_transform->rotation * (float)PI_DIV_180;
                float sin;
                float cos;
                get_sin_cos(rad, &sin, &cos);
                p_transform->position.x -= p_player->speed * sin * delta_time;
                p_transform->position.y -= p_player->speed * cos * delta_time;
            }

            if (get_key_state(VK_SPACE) == KEYBOARD_STATE_DOWN)
            {
                transform2_t missile_transform = *p_transform;
                missile_transform.scale = 16.0f;
                
                const ecs_id_t missile = gp_game->p_ecs->vtbl->create_entity(gp_game->p_ecs);
                gp_game->p_ecs->vtbl->set_component(gp_game->p_ecs, missile, gp_game->transform_component, &missile_transform);
                gp_game->p_ecs->vtbl->set_component(gp_game->p_ecs, missile, gp_game->mesh_component, &gp_game->p_missile_mesh);
                gp_game->p_ecs->vtbl->set_component(gp_game->p_ecs, missile, gp_game->missile_component,
                                                    &(missile_t){ missile_transform.position, 1000.0f, 250.0f });

                gp_game->p_missile_mesh->vtbl->add_ref(gp_game->p_missile_mesh);

                on_key_up(VK_SPACE);
            }

            gp_game->main_camera.transform.position = p_transform->position;
            gp_game->main_camera.view_matrix = matrix_set(1.0f, 0.0f, -p_transform->position.x, 0.0f,
                                                          0.0f, 1.0f, -p_transform->position.y, 0.0f,
                                                          0.0f, 0.0f, 1.0f, 0.0f,
                                                          0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
}

void control_missile_system(const ecs_view_t* p_view)
{
    const float delta_time = gp_game->p_renderer->vtbl->get_delta_time(gp_game->p_renderer);

    for (size_t i = 0; i < p_view->num_archetypes; ++i)
    {
        transform2_t* p_transforms = (transform2_t*)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->transform_component);
        missile_t* p_missiles = (missile_t*)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->missile_component);
        i_mesh2_t** pp_meshs = (i_mesh2_t**)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->mesh_component);

        const size_t num_instances = p_view->p_this->vtbl->get_num_instances(p_view, i);
        ecs_id_t* entities = (ecs_id_t*)gp_game->p_ecs->vtbl->get_entities_or_null(p_view, i);
        for (size_t j = 0; j < num_instances; ++j)
        {
            transform2_t* p_transform = p_transforms + j;
            missile_t* p_missile = p_missiles + j;
            i_mesh2_t* p_mesh = pp_meshs[j];

            const float rad = p_transform->rotation * (float)PI_DIV_180;
            float sin;
            float cos;
            get_sin_cos(rad, &sin, &cos);
            p_transform->position.x += p_missile->speed * sin * delta_time;
            p_transform->position.y += p_missile->speed * cos * delta_time;

            const vector2_t distance = { ABS(p_transform->position.x - p_missile->start_pos.x), ABS(p_transform->position.y - p_missile->start_pos.y) };
            if (vector_get_length(vector2_to_vector(&distance)) >= p_missile->max_distance)
            {
                SAFE_RELEASE(p_mesh);
                gp_game->p_ecs->vtbl->destroy_entity(gp_game->p_ecs, entities[j]);
            }
        }
    }
}