﻿//***************************************************************************
// 
// 파일: game.c
// 
// 설명: game
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/18
// 
//***************************************************************************

#include "precompiled.h"
#include "game.h"

typedef struct vertex
{
    vector2_t position;
    vector2_t texcoord;
} vertex_t;

// 전역 변수
extern HWND g_hwnd;

// 전역 함수
static void create_vertex_buffers(void);
static void create_index_buffers(void);
static void create_meshs(void);

bool init_game(void)
{
    srand((uint_t)time(NULL));

    memset(gp_game, 0, sizeof(game_t));

    // DLL 로드
    {
    #if defined(_WIN64)
        gp_game->h_renderer_dll = LoadLibrary(L"safe99_soft_renderer_x64.dll");
        gp_game->h_file_system_dll = LoadLibrary(L"safe99_file_system_x64.dll");
        gp_game->h_geometry_dll = LoadLibrary(L"safe99_geometry_x64.dll");
    #else
        gp_game->h_renderer_dll = LoadLibrary(L"safe99_soft_renderer_x86.dll");
        gp_game->h_file_system_dll = LoadLibrary(L"safe99_file_system_x86.dll");
        gp_game->h_ecs_dll = LoadLibrary(L"safe99_geometry_x86.dll");
    #endif // _WIN64

        if (gp_game->h_renderer_dll == NULL
            || gp_game->h_file_system_dll == NULL
            || gp_game->h_geometry_dll == NULL)
        {
            ASSERT(false, "Failed load DLL");
            goto failed_init;
        }

        // 인스턴스 생성 함수
        create_instance_func pf_create_renderer = (create_instance_func)GetProcAddress(gp_game->h_renderer_dll, "create_instance");
        create_instance_func pf_create_file_system = (create_instance_func)GetProcAddress(gp_game->h_file_system_dll, "create_instance");
        create_instance_func pf_create_geometry = (create_instance_func)GetProcAddress(gp_game->h_geometry_dll, "create_instance");

        // 렌더러 초기화
        pf_create_renderer(&gp_game->p_renderer);
        if (!gp_game->p_renderer->vtbl->initialize(gp_game->p_renderer, g_hwnd, false))
        {
            ASSERT(false, "Failed init renderer");
            goto failed_init;
        }

        // 파일 시스템 초기화
        pf_create_file_system(&gp_game->p_file_system);
        if (!gp_game->p_file_system->vtbl->initialize(gp_game->p_file_system))
        {
            ASSERT(false, "Failed init file system");
            goto failed_init;
        }

        // 지오메트리 초기화
        pf_create_geometry(&gp_game->p_geometry);
        if (!gp_game->p_geometry->vtbl->initialize(gp_game->p_geometry))
        {
            ASSERT(false, "Failed init geometry");
            goto failed_init;
        }
    }

    // 게임 초기화
    {
        gp_game->b_running = true;
        gp_game->b_wireframe = false;

        timer_init(&gp_game->frame_timer);

        set_limit_frame_rate(1.0f / 60.0f);

        // 카메라 초기화
        transform2_set_position(&gp_game->main_camera.transform, (vector2_t){ 0.0f, 0.0f });
        transform2_set_rotation(&gp_game->main_camera.transform, 0.0f);
        transform2_set_scale(&gp_game->main_camera.transform, (vector2_t){ 1.0f, 1.0f });
        gp_game->main_camera.view_port_left_top.x = -1.0f;
        gp_game->main_camera.view_port_left_top.y = -1.0f;
        gp_game->main_camera.view_port_right_bottom.x = 1.0f;
        gp_game->main_camera.view_port_right_bottom.y = 1.0f;

        // 텍스쳐 로딩
        if (!gp_game->p_file_system->vtbl->load_a8r8g8b8_dds(gp_game->p_file_system, "..\\..\\sprites\\sprites.dds", &gp_game->p_texture))
        {
            MessageBox(NULL, L"Failed to load texture", L"Error", MB_OK | MB_ICONERROR);
            ASSERT(false, "Failed to load texture");
            goto failed_init;
        }

        // 버텍스 버퍼 생성
        create_vertex_buffers();

        // 인덱스 버퍼 생성
        create_index_buffers();

        // 메시 생성
        create_meshs();
    }

    // ecs 초기화
    {
        // 월드 초기화
        gp_game->p_ecs = gp_game->p_geometry->vtbl->get_ecs(gp_game->p_geometry);
        if (!gp_game->p_ecs->vtbl->initialize(gp_game->p_ecs, 2500, 10, 10))  // TODO: 매개 변수 수정하기
        {
            ASSERT(false, "Failed init ecs");
            goto failed_init;
        }

        // 컴포넌트 등록
        gp_game->transform_component = ECS_REGISTER_COMPONENT(gp_game->p_ecs, transform2_t);
        gp_game->mesh_component = ECS_REGISTER_COMPONENT(gp_game->p_ecs, i_mesh_t*);
        gp_game->player_component = ECS_REGISTER_COMPONENT(gp_game->p_ecs, player_t);
        gp_game->enemy1_component = ECS_REGISTER_COMPONENT(gp_game->p_ecs, enemy1_t);
        gp_game->enemy2_component = ECS_REGISTER_COMPONENT(gp_game->p_ecs, enemy2_t);
        gp_game->missile_component = ECS_REGISTER_COMPONENT(gp_game->p_ecs, missile_t);

        // 시스템 등록
        gp_game->init_player_system = ECS_REGISTER_SYSTEM(gp_game->p_ecs, init_player_system, 3,
                                                          gp_game->transform_component,
                                                          gp_game->mesh_component,
                                                          gp_game->player_component);
        gp_game->init_enemy1_system = ECS_REGISTER_SYSTEM(gp_game->p_ecs, init_enemy1_system, 3,
                                                          gp_game->transform_component,
                                                          gp_game->mesh_component,
                                                          gp_game->enemy1_component);
        gp_game->init_enemy2_system = ECS_REGISTER_SYSTEM(gp_game->p_ecs, init_enemy2_system, 3,
                                                          gp_game->transform_component,
                                                          gp_game->mesh_component,
                                                          gp_game->enemy2_component);
        gp_game->render_mesh_system = ECS_REGISTER_SYSTEM(gp_game->p_ecs, render_mesh_system, 2,
                                                          gp_game->transform_component,
                                                          gp_game->mesh_component);
        gp_game->control_player_system = ECS_REGISTER_SYSTEM(gp_game->p_ecs, control_player_system, 2,
                                                             gp_game->transform_component,
                                                             gp_game->player_component);
        gp_game->control_missile_system = ECS_REGISTER_SYSTEM(gp_game->p_ecs, control_missile_system, 2,
                                                              gp_game->transform_component,
                                                              gp_game->missile_component);
        gp_game->collide_system = ECS_REGISTER_SYSTEM(gp_game->p_ecs, collide_system, 2,
                                                      gp_game->transform_component,
                                                      gp_game->mesh_component);

        // 적 생성
        {
            // 적1
            for (size_t i = 0; i < NUM_MAX_ENEMY1; ++i)
            {
                gp_game->enemy1[i] = gp_game->p_ecs->vtbl->create_entity(gp_game->p_ecs);
                gp_game->p_ecs->vtbl->add_component(gp_game->p_ecs, gp_game->enemy1[i], 3,
                                                    gp_game->transform_component,
                                                    gp_game->mesh_component,
                                                    gp_game->enemy1_component);
            }

            //적2
            for (size_t i = 0; i < NUM_MAX_ENEMY2; ++i)
            {
                gp_game->enemy2[i] = gp_game->p_ecs->vtbl->create_entity(gp_game->p_ecs);
                gp_game->p_ecs->vtbl->add_component(gp_game->p_ecs, gp_game->enemy2[i], 3,
                                                    gp_game->transform_component,
                                                    gp_game->mesh_component,
                                                    gp_game->enemy2_component);
            }
        }

        // 플레이어 생성
        gp_game->player = gp_game->p_ecs->vtbl->create_entity(gp_game->p_ecs);
        gp_game->p_ecs->vtbl->add_component(gp_game->p_ecs, gp_game->player, 3,
                                            gp_game->transform_component,
                                            gp_game->mesh_component,
                                            gp_game->player_component);

        // ecs 엔티티 초기화
        gp_game->p_ecs->vtbl->update_system(gp_game->p_ecs, gp_game->init_player_system);
        gp_game->p_ecs->vtbl->update_system(gp_game->p_ecs, gp_game->init_enemy1_system);
        gp_game->p_ecs->vtbl->update_system(gp_game->p_ecs, gp_game->init_enemy2_system);
    }

    return true;

failed_init:
    shutdown_game();
    return false;
}

void shutdown_game(void)
{
    // 텍스쳐 해제
    SAFE_RELEASE(gp_game->p_texture);

    // 버텍스 버퍼 해제
    SAFE_RELEASE(gp_game->p_player_vertex_buffer);
    SAFE_RELEASE(gp_game->p_enemy1_vertex_buffer);
    SAFE_RELEASE(gp_game->p_enemy2_vertex_buffer);
    SAFE_RELEASE(gp_game->p_missile_vertex_buffer);

    // 인덱스 버퍼 해제
    SAFE_RELEASE(gp_game->p_index_buffer);

    // 메시 해제
    SAFE_RELEASE(gp_game->p_player_mesh);
    SAFE_RELEASE(gp_game->p_enemy1_mesh);
    SAFE_RELEASE(gp_game->p_enemy2_mesh);
    SAFE_RELEASE(gp_game->p_missile_mesh);

    // DLL 객체 해제
    SAFE_RELEASE(gp_game->p_renderer);
    SAFE_RELEASE(gp_game->p_file_system);
    SAFE_RELEASE(gp_game->p_geometry);

    // DLL 핸들 해제
    FreeLibrary(gp_game->h_renderer_dll);
    FreeLibrary(gp_game->h_file_system_dll);
    FreeLibrary(gp_game->h_geometry_dll);
}

void tick_game(void)
{
    static float start_time;

    const float end_time = timer_get_time(&gp_game->frame_timer);
    const float delta_time = end_time - start_time;

    gp_game->delta_time = end_time - start_time;
    start_time = end_time;

    // 업데이트 시작
    {
        if (get_key_state(VK_F1) == KEYBOARD_STATE_DOWN)
        {
            gp_game->b_wireframe = !gp_game->b_wireframe;
            on_key_down(VK_F1);
        }

        // ecs 시스템 업데이트
        gp_game->p_ecs->vtbl->update_system(gp_game->p_ecs, gp_game->control_player_system);
        gp_game->p_ecs->vtbl->update_system(gp_game->p_ecs, gp_game->control_missile_system);
        gp_game->p_ecs->vtbl->update_system(gp_game->p_ecs, gp_game->collide_system);
    }

    // 렌더링 시작
    {
        // 화면 렌더링
        {
            gp_game->p_renderer->vtbl->clear(gp_game->p_renderer, color_set(0.0f, 0.0f, 0.0f, 1.0f));

            // 메시 렌더링
            gp_game->p_ecs->vtbl->update_system(gp_game->p_ecs, gp_game->render_mesh_system);
        }

        // 텍스트 정보 출력
        {
            static const int start_text_x = 1;
            static const int start_text_y = 1;
            static const int line_spacing = 16;

            // FPS 출력
            static wchar_t buffer[64];
            swprintf(buffer, 64, L"FPS: %zd", (size_t)(ROUND(1.0f / gp_game->delta_time)));
            gp_game->p_renderer->vtbl->draw_text(gp_game->p_renderer, 1, 1, buffer, wcslen(buffer), color_set(1.0f, 0.0f, 0.0f, 1.0f));

            // Delta time 출력
            swprintf(buffer, 64, L"Delta time: %.8lf", gp_game->delta_time);
            gp_game->p_renderer->vtbl->draw_text(gp_game->p_renderer, 1, 1 * line_spacing, buffer, wcslen(buffer), color_set(1.0f, 0.0f, 0.0f, 1.0f));

            // 플레이어 정보 출력
            {
                transform2_t* p_transform = (transform2_t*)gp_game->p_ecs->vtbl->get_component_or_null(gp_game->p_ecs,
                                                                                                       gp_game->player,
                                                                                                       gp_game->transform_component);
                ASSERT(p_transform != NULL, "p_transform == NULL");

                swprintf(buffer, 64, L"Position: [%.3f, %.3f]", p_transform->position.x, p_transform->position.y);
                gp_game->p_renderer->vtbl->draw_text(gp_game->p_renderer, 1, 2 * line_spacing, buffer, wcslen(buffer), color_set(1.0f, 0.0f, 0.0f, 1.0f));

                swprintf(buffer, 64, L"Rotation: %.3f Degree", p_transform->rotation);
                gp_game->p_renderer->vtbl->draw_text(gp_game->p_renderer, 1, 3 * line_spacing, buffer, wcslen(buffer), color_set(1.0f, 0.0f, 0.0f, 1.0f));

                swprintf(buffer, 64, L"Scale: x%.1f", p_transform->scale.x);
                gp_game->p_renderer->vtbl->draw_text(gp_game->p_renderer, 1, 4 * line_spacing, buffer, wcslen(buffer), color_set(1.0f, 0.0f, 0.0f, 1.0f));
            }
        }

        gp_game->p_renderer->vtbl->on_draw(gp_game->p_renderer);
    }
}

static void create_vertex_buffers(void)
{
    const float size = 0.5f;

    // 시멘틱
    const size_t offsets[2] = { 0, sizeof(vector2_t) };
    const semantic_t semantics[2] = { SEMANTIC_POSITION, SEMANTIC_TEXCOORD };
    const size_t num_semantics = sizeof(semantics) / sizeof(semantics[0]);

    // 플레이어 버텍스 버퍼 생성
    {
        // 버텍스 리스트
        const vertex_t vertices[] =
        {
            { { -size, -size }, { 0.00f, 0.99f } },
            { { -size, size }, { 0.00f, 0.00f } },
            { { size, size }, { 0.2941f, 0.00f } },
            { { size, -size }, { 0.2941f, 0.99f } },
        };
        const size_t num_vertices = sizeof(vertices) / sizeof(vertices[0]);

        // 버텍스 버퍼 생성
        gp_game->p_renderer->vtbl->create_vertex_buffer(gp_game->p_renderer, offsets, semantics, num_semantics,
                                                        vertices, num_vertices, &gp_game->p_player_vertex_buffer);
    }

    // 적1 버텍스 버퍼 생성
    {
        // 버텍스 리스트
        const vertex_t vertices[] =
        {
            { { -size, -size }, { 0.3137f, 0.99f } },
            { { -size, size }, { 0.3137f, 0.00f } },
            { { size, size }, { 0.6078f, 0.00f } },
            { { size, -size }, { 0.6078f, 0.99f } },
        };
        const size_t num_vertices = sizeof(vertices) / sizeof(vertices[0]);

        // 버텍스 버퍼 생성
        gp_game->p_renderer->vtbl->create_vertex_buffer(gp_game->p_renderer, offsets, semantics, num_semantics,
                                                        vertices, num_vertices, &gp_game->p_enemy1_vertex_buffer);
    }

    // 적2 버텍스 버퍼 생성
    {
        // 버텍스 리스트
        const vertex_t vertices[] =
        {
            { { -size, -size }, { 0.6274f, 0.99f } },
            { { -size, size }, { 0.6274f, 0.00f } },
            { { size, size }, { 0.9215f, 0.00f } },
            { { size, -size }, { 0.9215f, 0.99f } },
        };
        const size_t num_vertices = sizeof(vertices) / sizeof(vertices[0]);

        // 버텍스 버퍼 생성
        gp_game->p_renderer->vtbl->create_vertex_buffer(gp_game->p_renderer, offsets, semantics, num_semantics,
                                                        vertices, num_vertices, &gp_game->p_enemy2_vertex_buffer);
    }

    // 미사일 버텍스 버퍼 생성
    {
        // 버텍스 리스트
        vertex_t vertices[] =
        {
            { { -size, -size }, { 0.94f, 0.50f } },
            { { -size, size }, { 0.94f, 0.00f } },
            { { size, size }, { 1.00f, 0.00f } },
            { { size, -size }, { 1.00f, 0.50f } },
        };
        const size_t num_vertices = sizeof(vertices) / sizeof(vertices[0]);

        // 버텍스 버퍼 생성
        gp_game->p_renderer->vtbl->create_vertex_buffer(gp_game->p_renderer, offsets, semantics, num_semantics,
                                                        vertices, num_vertices, &gp_game->p_missile_vertex_buffer);
    }
}

static void create_index_buffers(void)
{
    // 인덱스 리스트
    uint_t indices[] =
    {
        0, 1, 2,
        0, 2, 3
    };
    const size_t num_indices = sizeof(indices) / sizeof(indices[0]);

    // 인덱스 버퍼 생성
    gp_game->p_renderer->vtbl->create_index_buffer(gp_game->p_renderer, indices, num_indices, &gp_game->p_index_buffer);
}

static void create_meshs(void)
{
    // 플레이어 메시 생성
    gp_game->p_renderer->vtbl->create_mesh(gp_game->p_renderer,
                                           gp_game->p_player_vertex_buffer,
                                           gp_game->p_index_buffer,
                                           gp_game->p_texture,
                                           color_set(0.0f, 1.0f, 0.0f, 1.0f),
                                           &gp_game->p_player_mesh);

    // 적1 메시 생성
    gp_game->p_renderer->vtbl->create_mesh(gp_game->p_renderer,
                                           gp_game->p_enemy1_vertex_buffer,
                                           gp_game->p_index_buffer,
                                           gp_game->p_texture,
                                           color_set(0.0f, 1.0f, 0.0f, 1.0f),
                                           &gp_game->p_enemy1_mesh);

    // 적2 메시 생성
    gp_game->p_renderer->vtbl->create_mesh(gp_game->p_renderer,
                                           gp_game->p_enemy2_vertex_buffer,
                                           gp_game->p_index_buffer,
                                           gp_game->p_texture,
                                           color_set(0.0f, 1.0f, 0.0f, 1.0f),
                                           &gp_game->p_enemy2_mesh);

    // 미사일 메시 생성
    gp_game->p_renderer->vtbl->create_mesh(gp_game->p_renderer,
                                           gp_game->p_missile_vertex_buffer,
                                           gp_game->p_index_buffer,
                                           gp_game->p_texture,
                                           color_set(0.0f, 1.0f, 0.0f, 1.0f),
                                           &gp_game->p_missile_mesh);
}