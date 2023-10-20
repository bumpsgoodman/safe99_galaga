//***************************************************************************
// 
// 파일: game.h
// 
// 설명: game
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/17
// 
//***************************************************************************

#ifndef GAME_H
#define GAME_H

#include "safe99_common/defines.h"
#include "safe99_ecs/i_ecs.h"
#include "safe99_file_system/i_file_system.h"
#include "safe99_math/math.h"
#include "safe99_soft_renderer_2d/i_soft_renderer_2d.h"

#define NUM_MAX_ENEMY1 1000
#define NUM_MAX_ENEMY2 1000

typedef struct game
{
    // DLL 핸들
    HINSTANCE h_renderer_dll;
    HINSTANCE h_file_system_dll;
    HINSTANCE h_ecs_dll;

    // DLL 인스턴스
    i_soft_renderer_2d_t* p_renderer;
    i_file_system_t* p_file_system;
    i_ecs_t* p_ecs;

    // 게임 관련
    bool b_running;
    bool b_wireframe;
    float limit_frame_rate;
    
    timer_t frame_timer;
    float delta_time;

    camera2_t main_camera;

    i_texture2_t* p_texture;

    i_vertex_buffer2_t* p_player_vertex_buffer;
    i_vertex_buffer2_t* p_enemy1_vertex_buffer;
    i_vertex_buffer2_t* p_enemy2_vertex_buffer;
    i_vertex_buffer2_t* p_missile_vertex_buffer;

    i_index_buffer2_t* p_index_buffer;

    i_mesh2_t* p_player_mesh;
    i_mesh2_t* p_enemy1_mesh;
    i_mesh2_t* p_enemy2_mesh;
    i_mesh2_t* p_missile_mesh;

    // ecs 엔티티
    ecs_id_t player;
    ecs_id_t enemy1[NUM_MAX_ENEMY1];
    ecs_id_t enemy2[NUM_MAX_ENEMY2];

    // ecs 컴포넌트
    ecs_id_t transform_component;
    ecs_id_t mesh_component;
    ecs_id_t player_component;
    ecs_id_t enemy1_component;
    ecs_id_t enemy2_component;
    ecs_id_t missile_component;

    // ecs 시스템
    ecs_id_t init_player_system;
    ecs_id_t init_enemy1_system;
    ecs_id_t init_enemy2_system;
    ecs_id_t render_mesh_system;
    ecs_id_t control_player_system;
    ecs_id_t control_missile_system;
    ecs_id_t collide_system;
} game_t;

// 전역 변수
game_t* gp_game;

// 전역 함수
bool init_game(void);
void shutdown_game(void);
void tick_game(void);

FORCEINLINE bool is_running_game(void)
{
    return gp_game->b_running;
}

FORCEINLINE void update_window_pos_game(void)
{
    gp_game->p_renderer->vtbl->update_window_pos(gp_game->p_renderer);
}

FORCEINLINE void update_window_size_game(void)
{
    gp_game->p_renderer->vtbl->update_window_size(gp_game->p_renderer);
}

FORCEINLINE void set_limit_frame_rate(const float rate)
{
    gp_game->limit_frame_rate = rate;
}

FORCEINLINE float get_limit_frame_rate(void)
{
    return gp_game->limit_frame_rate;
}

FORCEINLINE float get_delta_time_game(void)
{
    return gp_game->delta_time;
}

FORCEINLINE size_t get_fps_game(void)
{
    return (size_t)ROUND_INT(1.0f / gp_game->delta_time);
}

#endif // GAME_H