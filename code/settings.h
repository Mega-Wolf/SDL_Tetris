#pragma once

#define FPS 60
#define DELTA_TIME (    1 / (float) FPS)

// NOTE: x-offset; change when using a second monitor
//#define MONITOR_OFFSET 1920

// TODO: This doesn't quite belong here
#define MAX_Y 22
#define MAX_X 12

#define WINDOW_WIDTH ((MAX_X + 6) * 32)
#define WINDOW_HEIGHT (MAX_Y * 32)

#ifndef MONITOR_OFFSET
    #define MONITOR_OFFSET 0
#endif
