//***************************************************************************
// 
// 파일: render_system.c
// 
// 설명: ecs 렌더링 시스템
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/18
// 
//***************************************************************************

#include "precompiled.h"
#include "game.h"

void render_mesh_system(const ecs_view_t* p_view)
{
    for (size_t i = 0; i < p_view->num_archetypes; ++i)
    {
        const i_mesh2_t** pp_mesh = (i_mesh2_t**)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->mesh_component);
        const transform2_t* p_transforms = (transform2_t*)p_view->p_this->vtbl->get_instances_or_null(p_view, i, gp_game->transform_component);

        const size_t num_instances = p_view->p_this->vtbl->get_num_instances(p_view, i);
        for (size_t j = 0; j < num_instances; ++j)
        {
            const i_mesh2_t* p_mesh = pp_mesh[j];
            const transform2_t* p_transform = p_transforms + j;

            float sin;
            float cos;
            get_sin_cos(p_transform->rotation * (float)PI_DIV_180, &sin, &cos);
            const matrix_t world_mat = matrix_set(cos * p_transform->scale, sin * p_transform->scale, p_transform->position.x, 0.0f,
                                                  -sin * p_transform->scale, cos * p_transform->scale, p_transform->position.y, 0.0f,
                                                  0.0f, 0.0f, 1.0f, 0.0f,
                                                  0.0f, 0.0f, 0.0f, 1.0f);

            const matrix_t transform_mat = matrix_mul(gp_game->main_camera.view_matrix, matrix_get_transpose(world_mat));

            gp_game->p_renderer->vtbl->draw_mesh(gp_game->p_renderer, p_mesh, &transform_mat, gp_game->b_wireframe);
        }
    }
}