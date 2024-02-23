#include <iostream>
#include <windows.h>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
// #include <SDL2/SDL_mixer.h>
// #include <SDL2/SDL_tff.h>

#include "BaseObject.h"

static SDL_Window* g_window = NULL;
static SDL_Renderer* g_screen = NULL;
static SDL_Event g_event;

//Screen
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

//Chỉ số pixel của màn hình
const int SCREEN_BPP = 32;

const int COLOR_KEY_R = 167;
const int COLOR_KEY_G = 175;
const int COLOR_KEY_B = 180;

const int RENDERER_DRAWN_COLOR = 0xff;
BaseObject g_background;

bool init() 
{
    bool success = true;

    //Initialize SDL
    int ret = SDL_Init( SDL_INIT_EVERYTHING );
    if (ret<0) return false;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    //Create window
    g_window = SDL_CreateWindow( "PongX", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

    if ( g_window == NULL )
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else 
    {
        //Get window surface
        g_screen = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
        if (g_screen == NULL){
            success = false;
        }
        else
        {
            SDL_SetRenderDrawColor(g_screen, RENDERER_DRAWN_COLOR, RENDERER_DRAWN_COLOR, RENDERER_DRAWN_COLOR, RENDERER_DRAWN_COLOR);

            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) && imgFlags)) 
                success = false;
        }
    }

    return success;
}

bool LoadBackground()
{
    bool ret = g_background.LoadImg("assets/pong-x-bg.png", g_screen);
    return ret;
}


int main( int argc, char *argv[] )
{
    if (init() == false)
        return -1;

    if (LoadBackground() == false)
        return -1;

    bool quit = false;
    while (!quit)
    {
        if( SDL_PollEvent( &g_event ) )
        { 
            if( g_event.type == SDL_QUIT ) break;
        } 
    }



    return 0;
}