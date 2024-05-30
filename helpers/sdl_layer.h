//#include <stdio.h>
#pragma once

#include "../code/settings.h"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "../extern/glad/glad.c"
#include <SDL_opengl.h>

#include "backend.h"

// TODO: Maybe group the "ifndef PLATFORM_NO_AUDIO" stuff better

void Init();
void Update(color32* array, int width, int height, inputs* inputs);
void Exit();

#ifndef PLATFORM_NO_AUDIO
    void AudioCallback(void *userdata, unsigned char *stream8, int len);
#endif

static void ProcessKeyboard(button* button, bool isDown) {
    if (button->Down != isDown) {
        button->Down = isDown;
        button->Toggled = true;
    }
}

int main(int argc, char* argv[]) {
    UNUSED_PARAM(argc);
    UNUSED_PARAM(argv);

    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    #ifndef PLATFORM_NO_AUDIO
        SDL_Init(SDL_INIT_AUDIO);
    #endif

    SDL_Window* window = SDL_CreateWindow("", MONITOR_OFFSET + 1000, 50, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI |SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    // SDL_GLContext glContext =
    // TODO: Do I need this glContext somewhere?
    SDL_GL_CreateContext(window); // TODO: Do I need the OpenGL context
    gladLoadGL();

    SDL_Surface* window_surface = SDL_GetWindowSurface(window);

    #ifndef PLATFORM_NO_AUDIO
        SDL_AudioSpec desiredAudioSpec = {};
        desiredAudioSpec.channels = 2;
        desiredAudioSpec.format = AUDIO_S16SYS;
        desiredAudioSpec.freq = 48000;
        desiredAudioSpec.samples = 2048; // TODO: Not soo sure about this at the moment

        //desiredAudioSpec.padding; // TODO: Documentation doesn't mention this?!?
        //desiredAudioSpec.size;    // Don't set
        //desiredAudioSpec.silence; // Don't set
        desiredAudioSpec.callback = AudioCallback;
        //desiredAudioSpec.userdata; // Unused

        SDL_AudioSpec optainedAudioSpec;
        SDL_AudioDeviceID audioDeviceID = SDL_OpenAudioDevice(nullptr, false, &desiredAudioSpec, &optainedAudioSpec, 0);
    #endif
    
    unsigned char* keyboardStates = (unsigned char*) SDL_GetKeyboardState(nullptr);
    unsigned int * pixels = (unsigned int*) window_surface->pixels;
    int width = window_surface->w;
    int height = window_surface->h;

    Init();

    // Timing related stuff
    float gameUpdateHz = FPS; // / 2.0f; // TODO: Calc that
    float targetSecondsPerFrame = 1.0f / (float)gameUpdateHz;
    //GlobalPerfCountFrequency = SDL_GetPerformanceFrequency();
    unsigned long long lastCounter = SDL_GetPerformanceCounter();

    #ifndef PLATFORM_NO_AUDIO
        SDL_PauseAudioDevice(audioDeviceID, false);
    #endif

    //SDL_StartTextInput();

    Running = true;
    while (Running) {
        Inputs.TextInputLength = 0;
        Inputs.Keyboard.Backspace = false;
        Inputs.Keyboard.Delete = false;

        // Synchronise frame rate
        // TODO: I think this is not quite as it should be; I should add up stuff to actually get a correct speed
        {
            unsigned long long now = SDL_GetPerformanceCounter();

            float elapsedMS = (now - lastCounter) / (float) SDL_GetPerformanceFrequency() * 1000.0f;
            lastCounter = now;

            if (elapsedMS < targetSecondsPerFrame * 1000.0f) {
                unsigned int wait = (unsigned int) floorf(targetSecondsPerFrame * 1000.0f - elapsedMS);
                SDL_Delay(wait);
            }

        }

        Inputs.Mouse.WheelDelta = 0;
        for (int button_i = 0; button_i < PLATFORM_MOUSE_BUTTON_COUNT; ++button_i) {
            Inputs.Mouse.Buttons[button_i].Toggled = false;
        }
        for (int button_i = 0; button_i < PLATFORM_KEYBOARD_BUTTON_COUNT; ++button_i) {
            Inputs.Keyboard.Buttons[button_i].Toggled = false;
        }

        SDL_PumpEvents();

        // NOTE: This way, I don't miss KEY_UP events which might be the case when doing the other thing
        // I assume this comment is obsolete with SDL, but I keep it like that

        ProcessKeyboard(&Inputs.Keyboard.Up,    keyboardStates[SDL_SCANCODE_UP]    | keyboardStates[SDL_SCANCODE_W]);
        ProcessKeyboard(&Inputs.Keyboard.Right, keyboardStates[SDL_SCANCODE_RIGHT] | keyboardStates[SDL_SCANCODE_D]);
        ProcessKeyboard(&Inputs.Keyboard.Down,  keyboardStates[SDL_SCANCODE_DOWN]  | keyboardStates[SDL_SCANCODE_S]);
        ProcessKeyboard(&Inputs.Keyboard.Left,  keyboardStates[SDL_SCANCODE_LEFT]  | keyboardStates[SDL_SCANCODE_A]);

        // TODO(2022-04-21): Some of these should scan keycode instead of scancode I feel

        ProcessKeyboard(&Inputs.Keyboard.Enter,     keyboardStates[SDL_SCANCODE_RETURN]);
        ProcessKeyboard(&Inputs.Keyboard.Space,     keyboardStates[SDL_SCANCODE_SPACE]);
        ProcessKeyboard(&Inputs.Keyboard.Escape,    keyboardStates[SDL_SCANCODE_ESCAPE]);
        ProcessKeyboard(&Inputs.Keyboard.Shift,  keyboardStates[SDL_SCANCODE_LSHIFT]  | keyboardStates[SDL_SCANCODE_RSHIFT]);
        ProcessKeyboard(&Inputs.Keyboard.Ctrl,  keyboardStates[SDL_SCANCODE_LCTRL]  | keyboardStates[SDL_SCANCODE_RCTRL]);

        // TODO(2022-04-21): This is dumb, I can't have DELETE and BACKSPACE repeating at the moment, which would be quite important though

        // ProcessKeyboard(&Inputs.Keyboard.Backspace, keyboardStates[SDL_SCANCODE_BACKSPACE]);
        // ProcessKeyboard(&Inputs.Keyboard.Delete, keyboardStates[SDL_SCANCODE_DELETE]);
        ProcessKeyboard(&Inputs.Keyboard.Pos1, keyboardStates[SDL_SCANCODE_HOME]);
        ProcessKeyboard(&Inputs.Keyboard.End, keyboardStates[SDL_SCANCODE_END]);

        ProcessKeyboard(&Inputs.Keyboard.Num0, keyboardStates[SDL_SCANCODE_0]);
        ProcessKeyboard(&Inputs.Keyboard.Num1,  keyboardStates[SDL_SCANCODE_1]);
        ProcessKeyboard(&Inputs.Keyboard.Num2,  keyboardStates[SDL_SCANCODE_2]);
        ProcessKeyboard(&Inputs.Keyboard.Num3,  keyboardStates[SDL_SCANCODE_3]);
        ProcessKeyboard(&Inputs.Keyboard.Num4,  keyboardStates[SDL_SCANCODE_4]);
        ProcessKeyboard(&Inputs.Keyboard.Num5,  keyboardStates[SDL_SCANCODE_5]);
        ProcessKeyboard(&Inputs.Keyboard.Num6,  keyboardStates[SDL_SCANCODE_6]);
        ProcessKeyboard(&Inputs.Keyboard.Num7,  keyboardStates[SDL_SCANCODE_7]);
        ProcessKeyboard(&Inputs.Keyboard.Num8,  keyboardStates[SDL_SCANCODE_8]);
        ProcessKeyboard(&Inputs.Keyboard.Num9,  keyboardStates[SDL_SCANCODE_9]);

        ProcessKeyboard(&Inputs.Keyboard.F1,  keyboardStates[SDL_SCANCODE_F1]);
        ProcessKeyboard(&Inputs.Keyboard.F2,  keyboardStates[SDL_SCANCODE_F2]);
        ProcessKeyboard(&Inputs.Keyboard.F3,  keyboardStates[SDL_SCANCODE_F3]);
        ProcessKeyboard(&Inputs.Keyboard.F4,  keyboardStates[SDL_SCANCODE_F4]);
        ProcessKeyboard(&Inputs.Keyboard.F5,  keyboardStates[SDL_SCANCODE_F5]);
        ProcessKeyboard(&Inputs.Keyboard.F6,  keyboardStates[SDL_SCANCODE_F6]);
        ProcessKeyboard(&Inputs.Keyboard.F7,  keyboardStates[SDL_SCANCODE_F7]);
        ProcessKeyboard(&Inputs.Keyboard.F8,  keyboardStates[SDL_SCANCODE_F8]);
        ProcessKeyboard(&Inputs.Keyboard.F9,  keyboardStates[SDL_SCANCODE_F9]);
        ProcessKeyboard(&Inputs.Keyboard.F10, keyboardStates[SDL_SCANCODE_F10]);
        ProcessKeyboard(&Inputs.Keyboard.F11, keyboardStates[SDL_SCANCODE_F11]);
        ProcessKeyboard(&Inputs.Keyboard.F12, keyboardStates[SDL_SCANCODE_F12]);

        int mouseButtonState = SDL_GetMouseState(&Inputs.Mouse.PosX, &Inputs.Mouse.PosY);
        ProcessKeyboard(&Inputs.Mouse.Left,   (mouseButtonState & SDL_BUTTON(1)));
        ProcessKeyboard(&Inputs.Mouse.Middle, (mouseButtonState & SDL_BUTTON(2)));
        ProcessKeyboard(&Inputs.Mouse.Right,  (mouseButtonState & SDL_BUTTON(3)));

        SDL_Event event;
        while (Running && SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    Running = false;
                } break;
                case SDL_MOUSEWHEEL: {
                    // TODO(2022-04-24): x-input
                    Inputs.Mouse.WheelDelta = event.wheel.y;
                } break;
                case SDL_KEYDOWN: {
                    // NOTE(2022-04-22): This sends repeat keys (delayed)
                    switch (event.key.keysym.sym) {
                        case SDLK_RETURN: {
                            Inputs.TextInput[Inputs.TextInputLength++] = '\n';
                        } break;
                        case SDLK_BACKSPACE: {
                            Inputs.Keyboard.Backspace = true;
                        } break;
                        case SDLK_DELETE: {
                            Inputs.Keyboard.Delete = true;
                        } break;
                    }
                } break;
                case SDL_TEXTINPUT: {
                    int char_i = 0;
                    while(true) {
                        char c = event.text.text[char_i];
                        if (c == '\0') { break; }

                        if (c >= ' ' && c < ' ' + 96) {
                            // NOTE(2022-04-22): I can't render the other letters
                            Inputs.TextInput[Inputs.TextInputLength++] = c;
                        }
                        ++char_i;
                    }
                } break;
            }

            Inputs.TextInput[Inputs.TextInputLength] = '\0';
            
        }

        Update((color32*) pixels, width, height, &Inputs);

        SDL_UpdateWindowSurface(window);
    }

    // TODO: This freezes indefinitely quite often so I just don't care since it will be closed anyway
    //SDL_CloseAudioDevice(audioDeviceID);

    Exit();

    return 0;
}
