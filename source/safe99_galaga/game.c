//***************************************************************************
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
        gp_game->h_renderer_dll = LoadLibrary(L"safe99_soft_renderer_2d_x64.dll");
        gp_game->h_file_system_dll = LoadLibrary(L"safe99_file_system_x64.dll");
        gp_game->h_ecs_dll = LoadLibrary(L"safe99_ecs_x64.dll");
    #else
        gp_game->h_renderer_dll = LoadLibrary(L"safe99_soft_renderer_2d_x86.dll");
        gp_game->h_file_system_dll = LoadLibrary(L"safe99_file_system_x86.dll");
        gp_game->h_ecs_dll = LoadLibrary(L"safe99_ecs_x86.dll");
    #endif // _WIN64

        if (gp_game->h_renderer_dll == NULL
            || gp_game->h_file_system_dll == NULL
            || gp_game->h_ecs_dll == NULL)
        {
            ASSERT(false, "Failed load DLL");
            goto failed_init;
        }

        // 인스턴스 생성 함수
        create_instance_func pf_create_renderer = (create_instance_func)GetProcAddress(gp_game->h_renderer_dll, "create_instance");
        create_instance_func pf_create_file_system = (create_instance_func)GetProcAddress(gp_game->h_file_system_dll, "create_instance");
        create_instance_func pf_create_ecs = (create_instance_func)GetProcAddress(gp_game->h_ecs_dll, "create_instance");

        // 렌더러 초기화
        pf_create_renderer(&gp_game->p_renderer);
        if (!gp_game->p_renderer->vtbl->initialize(gp_game->p_renderer, g_hwnd))
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

        // ecs 월드 초기화
        pf_create_ecs(&gp_game->p_ecs);
        if (!gp_game->p_ecs->vtbl->initialize(gp_game->p_ecs, 10000, 100, 100))  // TODO: 매개 변수 수정하기
        {
            ASSERT(false, "Failed init ecs");
            goto failed_init;
        }
    }

    // 게임 초기화
    {
        gp_game->b_running = true;
        gp_game->b_wireframe = false;

        // 카메라 초기화
        gp_game->main_camera.transform.position.x = 0.0f;
        gp_game->main_camera.transform.position.y = 0.0f;
        gp_game->main_camera.view_matrix = matrix_get_identity();
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
        // 컴포넌트 등록
        gp_game->transform_component = ECS_REGISTER_COMPONENT(gp_game->p_ecs, transform2_t);
        gp_game->mesh_component = ECS_REGISTER_COMPONENT(gp_game->p_ecs, i_mesh2_t*);
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
        gp_game->release_mesh_system = ECS_REGISTER_SYSTEM(gp_game->p_ecs, release_mesh_system, 1,
                                                           gp_game->mesh_component);
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
    // ecs 엔티티 해제
    if (gp_game->release_mesh_system != 0)
    {
        gp_game->p_ecs->vtbl->update_system(gp_game->p_ecs, gp_game->release_mesh_system);
    }

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
    SAFE_RELEASE(gp_game->p_ecs);

    // DLL 핸들 해제
    FreeLibrary(gp_game->h_renderer_dll);
    FreeLibrary(gp_game->h_file_system_dll);
    FreeLibrary(gp_game->h_ecs_dll);
}

void tick_game(void)
{
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
        gp_game->p_renderer->vtbl->begin_draw(gp_game->p_renderer);
        {
            gp_game->p_renderer->vtbl->clear(gp_game->p_renderer, 0xff000000);

            // 메시 렌더링
            gp_game->p_ecs->vtbl->update_system(gp_game->p_ecs, gp_game->render_mesh_system);
        }
        gp_game->p_renderer->vtbl->end_draw(gp_game->p_renderer);

        // 텍스트 정보 출력
        HDC hdc;
        gp_game->p_renderer->vtbl->begin_gdi(gp_game->p_renderer, &hdc);
        {
            static wchar_t buffer[64];
            swprintf(buffer, 64, L"FPS: %zd", gp_game->p_renderer->vtbl->get_fps(gp_game->p_renderer));
            gp_game->p_renderer->vtbl->print_text(gp_game->p_renderer, hdc, buffer, 0, 0, wcslen(buffer), 0xffff0000);

            swprintf(buffer, 64, L"Delta time: %.8f", gp_game->p_renderer->vtbl->get_delta_time(gp_game->p_renderer));
            gp_game->p_renderer->vtbl->print_text(gp_game->p_renderer, hdc, buffer, 0, 15, wcslen(buffer), 0xffff0000);
        }
        gp_game->p_renderer->vtbl->end_gdi(gp_game->p_renderer, hdc);

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
        gp_game->p_renderer->vtbl->create_vertex_buffer(gp_game->p_renderer, semantics, offsets, num_semantics,
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
        gp_game->p_renderer->vtbl->create_vertex_buffer(gp_game->p_renderer, semantics, offsets, num_semantics,
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
        gp_game->p_renderer->vtbl->create_vertex_buffer(gp_game->p_renderer, semantics, offsets, num_semantics,
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
        gp_game->p_renderer->vtbl->create_vertex_buffer(gp_game->p_renderer, semantics, offsets, num_semantics,
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
                                           &gp_game->p_player_mesh);

    // 적1 메시 생성
    gp_game->p_renderer->vtbl->create_mesh(gp_game->p_renderer,
                                           gp_game->p_enemy1_vertex_buffer,
                                           gp_game->p_index_buffer,
                                           gp_game->p_texture,
                                           &gp_game->p_enemy1_mesh);

    // 적2 메시 생성
    gp_game->p_renderer->vtbl->create_mesh(gp_game->p_renderer,
                                           gp_game->p_enemy2_vertex_buffer,
                                           gp_game->p_index_buffer,
                                           gp_game->p_texture,
                                           &gp_game->p_enemy2_mesh);

    // 미사일 메시 생성
    gp_game->p_renderer->vtbl->create_mesh(gp_game->p_renderer,
                                           gp_game->p_missile_vertex_buffer,
                                           gp_game->p_index_buffer,
                                           gp_game->p_texture,
                                           &gp_game->p_missile_mesh);
}