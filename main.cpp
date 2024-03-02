#include <iostream>
#include <windows.h>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
// #include <SDL2/SDL_mixer.h>
// #include <SDL2/SDL_tff.h>

#include "header.h"

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
static SDL_Event g_event;

//Screen
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

//Chỉ số pixel của màn hình
const int SCREEN_BPP = 32;



LTexture::LTexture(){
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture(){
    free();
}

bool LTexture::loadFromFile(std::string path){
    free();

    SDL_Texture* newTexture = NULL;

    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    
    if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		// SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

int LTexture::getWidth(){
    return mWidth;
}

int LTexture::getHeight(){
    return mHeight;
}

void LTexture::render(int x, int y){
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};
    SDL_RenderCopy(gRenderer, mTexture, NULL, &renderQuad);
}

void LTexture::free(){
    if (mTexture != NULL){
        SDL_DestroyTexture(mTexture);
        mWidth = 0;
        mHeight = 0;
    }
}

bool init() 
{
    bool success = true;

    //Initialize SDL
    int ret = SDL_Init( SDL_INIT_EVERYTHING );
    if (ret<0) return false;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    //Create window
    gWindow = SDL_CreateWindow( "PongX", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

    if ( gWindow == NULL )
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else 
    {
        //Get window surface
        gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
        if (gRenderer == NULL){
            success = false;
        }
        else
        {
            SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);

            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) && imgFlags)) 
                success = false;
        }
    }

    return success;
}

//Texture list
LTexture gBackground;
bool loadMedia(){
    bool success = true;
    
    //Load game background
    if (!gBackground.loadFromFile("assets/pong-x-bg.png")){
        printf( "Failed to load background texture image!\n" );
		success = false;
    }


    return success;
}

int main( int argc, char *argv[] )
{
    if (init() == false){
        printf( "Failed to initialize!\n" );
        return -1;
    }

    if (loadMedia() == false){
        printf( "Failed to load media!\n" );
        return -1;
    }


    bool isRunning = true;
    while (isRunning)
    {
        if( SDL_PollEvent( &g_event ) )
        { 
            if( g_event.type == SDL_QUIT ) break;
        }
        //Clear screen
	    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
	    SDL_RenderClear( gRenderer );

        //Render game background
        gBackground.render(0,0);

        //Update screen
	    SDL_RenderPresent( gRenderer );

    }

    return 0;
}