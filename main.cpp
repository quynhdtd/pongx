#include <iostream>
#include <windows.h>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "header.h"

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
static SDL_Event g_event;
TTF_Font* gFont;
SDL_Color fColor;
bool isRunning = true;

//Screen
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int FONT_SIZE = 32;
const float PI = 3.14159265358979323846;


int frameCount, timerFPS, lastFrame, fps;

float velX, velY;
std::string score;
SDL_Rect scoreBoard;
int lSc, rSc;
bool turn;

//Texture list
LTexture gBackground;
LTexture topBar, botBar;
Paddle lPaddle, rPaddle;
Ball ball;


LTexture::LTexture(){
    mTexture = NULL;
    mWidth = mHeight = 0;
    mPosX = mPosY = 0;
    mVelX = mVelY = 0;
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

void LTexture::render(){
    SDL_Rect renderQuad = {mPosX, mPosY, mWidth, mHeight};
    SDL_RenderCopy(gRenderer, mTexture, NULL, &renderQuad);
}

void LTexture::free(){
    if (mTexture != NULL){
        SDL_DestroyTexture(mTexture);
        mWidth = 0;
        mHeight = 0;
    }
}

// void Paddle::movePaddle(int id){
//     const Uint8 *keystates = SDL_GetKeyboardState(NULL);

//     if (keystates[SDL_SCANCODE_UP]) rPaddle.mPosY -=  rPaddle.PAD_SPEED;
//     if (keystates[SDL_SCANCODE_DOWN]) rPaddle.mPosY += rPaddle.PAD_SPEED;
//     if (keystates[SDL_SCANCODE_LEFT]) rPaddle.mPosX -= rPaddle.PAD_SPEED;
//     if (keystates[SDL_SCANCODE_RIGHT]) rPaddle.mPosX += rPaddle.PAD_SPEED;

//     if (keystates[SDL_SCANCODE_W]) lPaddle.mPosY -=  lPaddle.PAD_SPEED;
//     if (keystates[SDL_SCANCODE_S]) lPaddle.mPosY += lPaddle.PAD_SPEED;
//     if (keystates[SDL_SCANCODE_A]) lPaddle.mPosX -= lPaddle.PAD_SPEED;
//     if (keystates[SDL_SCANCODE_D]) lPaddle.mPosX += lPaddle.PAD_SPEED;
// }

bool init() {
    bool success = true;

    //Initialize SDL
    int ret = SDL_Init( SDL_INIT_EVERYTHING );
    if (ret<0) return false;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    //Create window
    gWindow = SDL_CreateWindow( "PongX", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

    if ( gWindow == NULL ) {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    } else {
        //Get window surface
        gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
        if (gRenderer == NULL){
            success = false;
        } else {
            SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);

            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) && imgFlags)) 
                success = false;

            if (TTF_Init() == -1){
                printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                success = false;
            }
        }
    }

    return success;
}

//reset paddle pos
void serve() {
    lPaddle.mPosY = rPaddle.mPosY = SCREEN_HEIGHT/2 - lPaddle.getHeight()/2;
    if (turn){
        ball.mPosX = lPaddle.mPosX + (lPaddle.getWidth()*4);
        ball.mVelX = ball.BALL_SPEED/2;
    } else {
        ball.mPosX = rPaddle.mPosX - (rPaddle.getWidth()*4);
        ball.mVelX = -ball.BALL_SPEED/2;
    }

    ball.mVelY = 0;
    ball.mPosY = SCREEN_HEIGHT/2 - (ball.BALL_SIZE/2);
    turn = !turn;
}

//write score to screen
void write(std::string text, int x, int y){
    SDL_Surface* surface;
    SDL_Texture* texture;
    const char* t = text.c_str();
    surface = TTF_RenderText_Solid(gFont, t, fColor);
    texture = SDL_CreateTextureFromSurface(gRenderer, surface);
    scoreBoard.w = surface->w;
    scoreBoard.h = surface->h;
    scoreBoard.x = x - surface->w;
    scoreBoard.y = y - surface->h;
    SDL_FreeSurface(surface);
    SDL_RenderCopy(gRenderer, texture, NULL, &scoreBoard);
    SDL_DestroyTexture(texture);
}

bool loadMedia(){
    bool success = true;
    
    //Load game background
    if (!gBackground.loadFromFile("assets/pong-x-bg.png")){
        printf( "Failed to load background texture image!\n" );
		success = false;
    }

    //Load game font
    gFont = TTF_OpenFont("assets/Peepo.ttf", FONT_SIZE);
    if (gFont == NULL){
        printf( "Failed to load peepo font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }

    //Load paddle
    if (!lPaddle.loadFromFile("assets/pong-left-paddle-def.png")){
        printf( "Failed to load paddle texture image!\n" );
		success = false;
    }
    if (!rPaddle.loadFromFile("assets/pong-right-paddle-def.png")){
        printf( "Failed to load paddle texture image!\n" );
		success = false;
    }


    return success;
}

void update(){
    score = std::to_string(lSc) + " " + std::to_string(rSc);
}

void input(){
    SDL_Event e;
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    while (SDL_PollEvent(&e))
        if (e.type==SDL_QUIT) isRunning = false;

    if (keystates[SDL_SCANCODE_UP]) rPaddle.mPosY -=  rPaddle.PAD_SPEED;
    if (keystates[SDL_SCANCODE_DOWN]) rPaddle.mPosY += rPaddle.PAD_SPEED;
    if (keystates[SDL_SCANCODE_LEFT]) rPaddle.mPosX -= rPaddle.PAD_SPEED;
    if (keystates[SDL_SCANCODE_RIGHT]) rPaddle.mPosX += rPaddle.PAD_SPEED;

    if (keystates[SDL_SCANCODE_W]) lPaddle.mPosY -=  lPaddle.PAD_SPEED;
    if (keystates[SDL_SCANCODE_S]) lPaddle.mPosY += lPaddle.PAD_SPEED;
    if (keystates[SDL_SCANCODE_A]) lPaddle.mPosX -= lPaddle.PAD_SPEED;
    if (keystates[SDL_SCANCODE_D]) lPaddle.mPosX += lPaddle.PAD_SPEED;
}


void renderToScreen() {
    //clear screen
    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear( gRenderer );

    frameCount++;
    timerFPS = SDL_GetTicks() - lastFrame;
    if (timerFPS < (1000/60)){
        SDL_Delay((1000/60) - timerFPS);
    }

    //render here
    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

    //render game background
    gBackground.render();

    //render paddle
    lPaddle.render();
    rPaddle.render();

    //render ball
    //ball.render(0, 0);

    write(score, SCREEN_WIDTH/2 + FONT_SIZE, FONT_SIZE*2);
    //update to screen
    SDL_RenderPresent( gRenderer );

}



void close(){
    TTF_CloseFont(gFont);
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
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

    static int lastTime = 0;

    fColor.r = fColor.g = fColor.b = 255;
    lSc = rSc = 0;
    lPaddle.mPosY = rPaddle.mPosY = SCREEN_HEIGHT/2 - lPaddle.getHeight()/2;
    lPaddle.mPosX = 32;
    rPaddle.mPosX = SCREEN_WIDTH - rPaddle.getWidth() - 32;
    
    serve();

    while (isRunning)
    {
        lastFrame = SDL_GetTicks();
        if (lastFrame >= (lastTime+1000)){
            lastTime = lastFrame;
            fps=frameCount;
            frameCount=0;
        }
        
        update();
        input();
        renderToScreen();

        //Clear screen
	    // SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
	    // SDL_RenderClear( gRenderer );

        //Render game background
        // gBackground.render(0,0);

        //Update screen
	    // SDL_RenderPresent( gRenderer );

    }

    close();
    return 0;
}