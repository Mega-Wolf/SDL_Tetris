#include "settings.h"

#include <stdlib.h>

#define PLATFORM_NO_AUDIO

#include "../helpers/sdl_layer.h"

#define IS_KEY_PRESSED(key) ins->Keyboard.key.Down && ins->Keyboard.key.Toggled

#define KEY_PAUSE Escape
#define KEY_ROTATE_CW F2
#define KEY_ROTATE_CCW F1

// Global Variables
color32* Array;
int ArrayWidth;

int FrameCount;

color32 Stones[MAX_Y][MAX_X];

struct tetromino {
    color32 Color;
    bool Data4x4[4][4][4]; // rotation - y - x
};

tetromino Tetrominos[] = {
    // I
    {
        RED,
        {
            {
                {     1,     1,    1,     1 },
                { false, false, false, false },
                { false, false, false, false },
                { false, false, false, false },
            },
            {
                { false,     1, false, false },
                { false,     1, false, false },
                { false,     1, false, false },
                { false,     1, false, false },
            },
            {
                {     1,     1,     1,     1 },
                { false, false, false, false },
                { false, false, false, false },
                { false, false, false, false },
            },
            {
                { false,     1, false, false },
                { false,     1, false, false },
                { false,     1, false, false },
                { false,     1, false, false },
            },
        }
    },
    // O
    {
        BLUE,
        {
            {
                { false,     1,     1, false },
                { false,     1,     1, false },
                { false, false, false, false },
                { false, false, false, false },
            },
            {
                { false,     1,     1, false },
                { false,     1,     1, false },
                { false, false, false, false },
                { false, false, false, false },
            },
            {
                { false,     1,     1, false },
                { false,     1,     1, false },
                { false, false, false, false },
                { false, false, false, false },
            },
            {
                { false,     1,     1, false },
                { false,     1,     1, false },
                { false, false, false, false },
                { false, false, false, false },
            },
        }
    },
    // J
    {
        YELLOW,
        {
            {
                {     1, false, false, false },
                {     1,     1,     1, false },
                { false, false, false, false },
                { false, false, false, false },
            },
            {
                {     1,     1, false, false },
                {     1, false, false, false },
                {     1, false, false, false },
                { false, false, false, false },
            },
            {
                {     1,     1,     1, false },
                { false, false,     1, false },
                { false, false, false, false },
                { false, false, false, false },
            },
            {
                { false,     1, false, false },
                { false,     1, false, false },
                {     1,     1, false, false },
                { false, false, false, false },
            },
        }
    },
    // L
    {
        PINK,
        {
            {
                { false, false,     1, false },
                {     1,     1,     1, false },
                { false, false, false, false },
                { false, false, false, false },
            },
            {
                {     1, false, false, false },
                {     1, false, false, false },
                {     1,     1, false, false },
                { false, false, false, false },
            },
            {
                {     1,     1,     1, false },
                {     1, false, false, false },
                { false, false, false, false },
                { false, false, false, false },
            },
            {
                {     1,     1, false, false },
                { false,     1, false, false },
                { false,     1, false, false },
                { false, false, false, false },
            },
        }
    },
    // S
    {
        AQUA,
        {
            {
                { false,     1,     1, false },
                {     1,     1, false, false },
                { false, false, false, false },
                { false, false, false, false },
            },
            {
                {     1, false, false, false },
                {     1,     1, false, false },
                { false,     1, false, false },
                { false, false, false, false },
            },
            {
                { false,     1,     1, false },
                {     1,     1, false, false },
                { false, false, false, false },
                { false, false, false, false },
            },
            {
                {     1, false, false, false },
                {     1,     1, false, false },
                { false,     1, false, false },
                { false, false, false, false },
            },
        }
    },
    // Z
    {
        ORANGE,
        {
            {
                {     1,     1, false, false },
                { false,     1,     1, false },
                { false, false, false, false },
                { false, false, false, false },
            },
            {
                { false,     1, false, false },
                {     1,     1, false, false },
                {     1, false, false, false },
                { false, false, false, false },
            },
            {
                {     1,     1, false, false },
                { false,     1,     1, false },
                { false, false, false, false },
                { false, false, false, false },
            },
            {
                { false,     1, false, false },
                {     1,     1, false, false },
                {     1, false, false, false },
                { false, false, false, false },
            },
        }
    },
    // T
    {
        GREEN,
        {
            {
                {     1,     1,     1, false },
                { false,     1, false, false },
                { false, false, false, false },
                { false, false, false, false },
            },
            {
                { false,     1, false, false },
                {     1,     1, false, false },
                { false,     1, false, false },
                { false, false, false, false },
            },
            {
                { false,     1, false, false },
                {     1,     1,     1, false },
                { false, false, false, false },
                { false, false, false, false },
            },
            {
                {     1, false, false, false },
                {     1,     1, false, false },
                {     1, false, false, false },
                { false, false, false, false },
            },
        }
    },
};

int StoneStartX;
int StoneStartY;
int StoneIndex;
int NextStoneIndex;
int StoneRotation;

int RestTick = 1;
int UpdateTickMax = 20;

bool IsRunning = true;
bool IsGameOver = false;

void InitNewStone() {
    StoneStartX = (MAX_X - 1) / 2;
    StoneStartY = 1;
    StoneIndex = NextStoneIndex;
    NextStoneIndex = rand() % 7;
    StoneRotation = 0;
}

void Init() {
    srand(124);

    NextStoneIndex = rand() % 7;
    InitNewStone();

    for (int x_i = 0; x_i < MAX_X; ++x_i) {
        Stones[ 0][x_i] = LIGHT_GREY;
        Stones[MAX_Y - 1][x_i] = LIGHT_GREY;
    }

    for (int y_i = 1; y_i < MAX_Y - 1; ++y_i) {
        Stones[y_i][0] = LIGHT_GREY;
        Stones[y_i][MAX_X - 1] = LIGHT_GREY;
    }
}

int Index2D(int x, int y, int width) {
    return y * width + x;
}

void DrawRectangle(int x, int y, int width, int height, color32 col) {
    for (int y_i = y; y_i < y + height; ++y_i) {
        for (int x_i = x; x_i < x + width; ++x_i) {
            Array[Index2D(x_i, y_i, ArrayWidth)] = col;
        }
    }
}

void DrawBlock(int x, int y, color32 col) {
    DrawRectangle(x * 32 + 2, y * 32 + 2, 32 - 4, 32 - 4, col);
}

bool Check(int rotation, int offsetX, int offsetY) {
    for (int y_i = 0; y_i < 4; ++y_i) {
        for (int x_i = 0; x_i < 4; ++x_i) {
            if (Tetrominos[StoneIndex].Data4x4[rotation][y_i][x_i]) {
                if (Stones[StoneStartY + y_i + offsetY][StoneStartX + x_i + offsetX].Value) {
                    return false;
                }
            }
        }
    }

    return true;
}

void Update(color32* array, int width, int height, inputs* ins) {
    UNUSED_PARAM(height);

    Array = array;
    ArrayWidth = width;

    /// Pause
    if (IS_KEY_PRESSED(KEY_PAUSE)) {
        IsRunning = !IsRunning;

        if (IsRunning && IsGameOver) {
            IsGameOver = false;
            for (int y_i = 1; y_i < MAX_Y - 1; ++y_i) {
                for (int x_i = 1; x_i < MAX_X - 1; ++x_i) {
                    Stones[y_i][x_i] = C_TRANSPARENT;
                }
            }
        }
    }

    if (IsRunning) {

        /// Handle Input
        {
            /// Move Right
            if (IS_KEY_PRESSED(Right)) {
                bool movementPossible = Check(StoneRotation, 1, 0);

                if (movementPossible) {
                    ++StoneStartX;
                }
            }

            /// Move Left
            if (IS_KEY_PRESSED(Left)) {
                bool movementPossible = Check(StoneRotation, -1, 0);

                if (movementPossible) {
                    --StoneStartX;
                }
            }

            /// Rotate clockwise
            if (IS_KEY_PRESSED(KEY_ROTATE_CW)) {
                int proposedRotation = (StoneRotation + 1) % 4;
                bool rotationPossible = Check(proposedRotation, 0, 0);

                if (rotationPossible) {
                    StoneRotation = proposedRotation;
                }
            }

            /// Rotate counter-clockwise
            if (IS_KEY_PRESSED(KEY_ROTATE_CCW)) {
                int proposedRotation = (StoneRotation - 1 + 4) % 4;
                bool rotationPossible = Check(proposedRotation, 0, 0);

                if (rotationPossible) {
                    StoneRotation = proposedRotation;
                }
            }
        }

        // Logic Update
        {
            ++FrameCount;

            if (--RestTick == 0 || (ins->Keyboard.Down.Down && (FrameCount % 3) == 0)) {
                RestTick = UpdateTickMax;

                bool movementPossible = Check(StoneRotation, 0, 1);

                if (movementPossible) {
                    ++StoneStartY;
                } else {
                    for (int y_i = 0; y_i < 4; ++y_i) {
                        for (int x_i = 0; x_i < 4; ++x_i) {
                            if (Tetrominos[StoneIndex].Data4x4[StoneRotation][y_i][x_i]) {
                                Stones[StoneStartY + y_i][StoneStartX + x_i] = Tetrominos[StoneIndex].Color;
                            }
                        }
                    }

                    for (int row_i = MAX_Y - 2; row_i > 0; --row_i) {

                        bool rowCompletelyFilled = true;
                        for (int x_i = 1; x_i < MAX_X - 1; ++x_i) {
                            if (!Stones[row_i][x_i].Value) {
                                rowCompletelyFilled = false;
                                break;
                            }
                        }

                        if (rowCompletelyFilled) {
                            if (UpdateTickMax > 2) {
                                --UpdateTickMax;
                            }

                            /// Row should be cleared (=> everything else falls down)
                            for (int x_i = 1; x_i < MAX_X - 1; ++x_i) {
                                for (int y_i = row_i; y_i > 1; --y_i) {
                                    Stones[y_i][x_i] = Stones[y_i - 1][x_i];
                                }
                            }
                            ++row_i;
                        }
                    }

                    InitNewStone();

                    /// Check for game over
                    bool initStonePossible = Check(StoneRotation, 0, 0);

                    if (!initStonePossible) {
                        IsRunning = false;
                        IsGameOver = true;
                    }

                }
            }
        }

        /// Render
        {
            /// Clear screen
            DrawRectangle(0, 0, width, height, BLACK);

            /// Render Static Stones
            for (int y_i = 0; y_i < MAX_Y; ++y_i) {
                for (int x_i = 0; x_i < MAX_X; ++x_i) {
                    if (Stones[y_i][x_i].Value) {
                        DrawBlock(x_i, y_i, Stones[y_i][x_i]);
                    }
                }
            }

            /// Render Stone
            for (int y_i = 0; y_i < 4; ++y_i) {
                for (int x_i = 0; x_i < 4; ++x_i) {
                    if (Tetrominos[StoneIndex].Data4x4[StoneRotation][y_i][x_i]) {
                        DrawBlock(StoneStartX + x_i, StoneStartY + y_i, Tetrominos[StoneIndex].Color);
                    }
                }
            }

            /// Render Next Stone
            for (int y_i = 0; y_i < 4; ++y_i) {
                for (int x_i = 0; x_i < 4; ++x_i) {
                    if (Tetrominos[NextStoneIndex].Data4x4[0][y_i][x_i]) {
                        DrawBlock(MAX_X + 1 + x_i, 1 + y_i, Tetrominos[NextStoneIndex].Color);
                    }
                }
            }
        }
    }

}

void Exit() {
}
