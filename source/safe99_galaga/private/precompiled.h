//***************************************************************************
// 
// 파일: precompiled.h
// 
// 설명: precompiled 헤더
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/17
// 
//***************************************************************************

#ifndef PRECOMPIELD_H
#define PRECOMPIELD_H

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // _CRT_SECURE_NO_WARNINGS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <windowsx.h>

#include "safe99_common/defines.h"
#include "safe99_ecs/i_ecs.h"
#include "safe99_file_system/i_file_system.h"
#include "safe99_soft_renderer/i_soft_renderer.h"
#include "safe99_generic/util/timer.h"
#include "safe99_math/math.h"

#include "key_event.h"

#include "component/player.h"
#include "component/enemy1.h"
#include "component/enemy2.h"
#include "component/missile.h"

#include "system/init_system.h"
#include "system/render_system.h"
#include "system/control_system.h"
#include "system/collide_system.h"

#endif // PRECOMPIELD_H