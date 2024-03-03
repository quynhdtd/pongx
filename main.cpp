#include <iostream>
#include <windows.h>
#include <string>
#include <math.h>
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
bool startBall = false;
bool opt = false;

//Screen
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int FONT_SIZE = 80;
const double PI = 3.14159265358979323846;


int frameCount, timerFPS, lastFrame, fps;

float velX, velY;
std::string score;
SDL_Rect scoreBoard;
int lSc, rSc;
bool turn;

//Texture list
LTexture gBackground;
LTexture gOpt;
Paddle lPaddle, rPaddle;
Ball ball;
SDL_Rect topBar = {0, 0, SCREEN_WIDTH, 8};
SDL_Rect topLeftBar = {0, 8, 8, 178};
SDL_Rect topRightBar = {SCREEN_WIDTH-8, 8, 8, 178};

SDL_Rect botBar = {0, SCREEN_HEIGHT-8, SCREEN_WIDTH, 8};
SDL_Rect botLeftBar = {0, SCREEN_HEIGHT - 186, 8, 178};
SDL_Rect botRightBar = {SCREEN_WIDTH-8, SCREEN_HEIGHT - 186, 8, 178};

SDL_Rect topLeftWall = {SCREEN_WIDTH/4, SCREEN_HEIGHT/4, 60, 60};
SDL_Rect botLeftWall = {SCREEN_WIDTH/4, SCREEN_HEIGHT*3/4-60, 60, 60};
SDL_Rect topRightWall = {SCREEN_WIDTH*3/4-60, SCREEN_HEIGHT/4, 60, 60};
SDL_Rect botRightWall = {SCREEN_WIDTH*3/4-60, SCREEN_HEIGHT*3/4-60, 60, 60};

//Button list
LButton playButton;
LButton paddlex1;
LButton paddlex2;
int gamemode = 0;



bool checkCollision( SDL_Rect a, SDL_Rect b )
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    //Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    //If any of the sides from A are outside of B
    if( bottomA <= topB )
        return false;

    if( topA >= bottomB )
        return false;

    if( rightA <= leftB )
        return false;

    if( leftA >= rightB )
        return false;

    //If none of the sides from A are outside B
    return true;
}

void bouncingBall(SDL_Rect rect, int tag){
    double rel = (rect.y + (rect.h/2) - (ball.mPosY + (ball.BALL_SIZE/2)));
    double norm = rel/(rect.h/2);
    double bounce = norm*(5*PI/12);
    ball.mVelX = tag*ball.BALL_SPEED*cos(bounce);
    ball.mVelY = ball.BALL_SPEED*-sin(bounce);
}

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

bool LButton::handleButton(SDL_Event* e){
    //Check if mouse is in button
    bool inside;
    if (e->type == SDL_BUTTON_LEFT){
        int x, y;
        SDL_GetMouseState( &x, &y );
        // std::cout<<x<<y;
        

        //Mouse is left of the button
        if( x < mPosX ){
            inside = false;
        }
        //Mouse is right of the button
        else if( x > mPosX + BUTTON_WIDTH )
        {
            inside = false;
        }
        //Mouse above the button
        else if( y < mPosY )
        {
            inside = false;
        }
        //Mouse below the button
        else if( y > mPosY + BUTTON_HEIGHT )
        {
            inside = false;
        }
    }
    return inside;
}

void Ball::moveBall(SDL_Rect &wall){
    //Move the dot left or right
    mPosX += mVelX;
	mCollider.x = mPosX;

    //If the dot collided or went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + BALL_SIZE > SCREEN_WIDTH ) || checkCollision( mCollider, wall ) )
    {
        //Move back
        mPosX -= mVelX;
		mCollider.x = mPosX;
    }

    //Move the dot up or down
    mPosY += mVelY;
	mCollider.y = mPosY;

    //If the dot collided or went too far up or down
    if( ( mPosY < 0 ) || ( mPosY + BALL_SIZE > SCREEN_HEIGHT ) || checkCollision( mCollider, wall ) )
    {
        //Move back
        mPosY -= mVelY;
		mCollider.y = mPosY;
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

//reset paddle pos, change turn
void serve() {
    startBall = false;
    lPaddle.mPosY = rPaddle.mPosY = SCREEN_HEIGHT/2 - lPaddle.getHeight()/2;
    lPaddle.mPosX = 32;
    rPaddle.mPosX = SCREEN_WIDTH - rPaddle.getWidth() - 32;

    if (turn){
        ball.mPosX = lPaddle.mPosX + (lPaddle.getWidth()*4);
        ball.mVelX = ball.BALL_SPEED/1.5;
    } else {
        ball.mPosX = rPaddle.mPosX - (rPaddle.getWidth()*4);
        ball.mVelX = -ball.BALL_SPEED/1.5;
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
    //Load game option board
    if (!gOpt.loadFromFile("assets/pong-x-opt.png")){
        printf( "Failed to load option board texture image!\n" );
		success = false;
    }
    //Load game button
    if (!playButton.loadFromFile("assets/pong-x-play-button.png")){
        printf( "Failed to load play button texture image!\n" );
		success = false;
    }
    if (!paddlex1.loadFromFile("assets/pong-x-1-paddle-button.png")){
        printf( "Failed to load paddlex1 button texture image!\n" );
		success = false;
    }
    if (!paddlex2.loadFromFile("assets/pong-x-2-paddle-button.png")){
        printf( "Failed to load paddlex2 button texture image!\n" );
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
        printf( "Failed to load left paddle texture image!\n" );
		success = false;
    }
    if (!rPaddle.loadFromFile("assets/pong-right-paddle-def.png")){
        printf( "Failed to load right paddle texture image!\n" );
		success = false;
    }

    //Load ball
    if(!ball.loadFromFile("assets/pong-ball.png")){
        printf( "Failed to load ball texture image!\n" );
		success = false;
    }


    return success;
}

void update(){
    SDL_Rect ballRect = {ball.mPosX, ball.mPosY, ball.BALL_SIZE, ball.BALL_SIZE};
    SDL_Rect rPaddleRect = {rPaddle.mPosX, rPaddle.mPosY, rPaddle.getWidth(), rPaddle.getHeight()};
    SDL_Rect lPaddleRect = {lPaddle.mPosX, lPaddle.mPosY, lPaddle.getWidth(), lPaddle.getHeight()};
    if (checkCollision(ballRect, rPaddleRect)) {
        double rel = (rPaddle.mPosY + (rPaddle.getHeight()/2) - (ball.mPosY + (ball.BALL_SIZE/2)));
        double norm = rel/(rPaddle.getHeight()/2);
        double bounce = norm*(5*PI/12);
        ball.mVelX = -ball.BALL_SPEED*cos(bounce);
        ball.mVelY = ball.BALL_SPEED*-sin(bounce);
    }

    if (checkCollision(ballRect, lPaddleRect)) {
        double rel = (lPaddle.mPosY + (lPaddle.getHeight()/2) - (ball.mPosY + (ball.BALL_SIZE/2)));
        double norm = rel/(lPaddle.getHeight()/2);
        double bounce = norm*(5*PI/12);
        ball.mVelX = ball.BALL_SPEED*cos(bounce);
        ball.mVelY = ball.BALL_SPEED*-sin(bounce);
    }

    if (checkCollision(ballRect, topLeftWall)) bouncingBall(topLeftWall, 1);
    if (checkCollision(ballRect, botLeftWall)) bouncingBall(botLeftWall, 1);
    if (checkCollision(ballRect, topRightWall)) bouncingBall(topRightWall, -1);
    if (checkCollision(ballRect, botRightWall)) bouncingBall(botRightWall, -1);

    ballRect.x = ball.mPosX;
    ballRect.y = ball.mPosY;
    bool side = true;

    if (checkCollision(ballRect, topLeftBar) || checkCollision(ballRect, topRightBar)
        || checkCollision(ballRect, botLeftBar) || checkCollision(ballRect, botRightBar)){
        ball.mVelX = -ball.mVelX;
        side = false;        
    }
        
    if (checkCollision(ballRect, topBar) || checkCollision(ballRect, botBar)) 
        ball.mVelY = -ball.mVelY;

    
    //auto move right paddle
    if (ball.mPosY > rPaddle.mPosY + (rPaddle.getHeight()/2))
        rPaddle.mPosY += rPaddle.PAD_SPEED;
    if (ball.mPosY < rPaddle.mPosY + (rPaddle.getHeight()/2))
        rPaddle.mPosY -= rPaddle.PAD_SPEED;

    //move ball
    if (ball.mPosX <= 0 && side) {
        rSc++;
        serve();
    }
    if (ball.mPosX + ball.BALL_SIZE >= SCREEN_WIDTH && side){
        lSc++;
        serve();
    }

    if (startBall == true){
        ball.mPosX+=ball.mVelX;
        ball.mPosY+=ball.mVelY;
    }

    score = std::to_string(lSc) + "   " + std::to_string(rSc);

    //Make sure paddle don't fly out game screen
    if (lPaddle.mPosY < 0) 
        lPaddle.mPosY = 0;
    if (lPaddle.mPosY + lPaddle.getHeight() > SCREEN_HEIGHT)
        lPaddle.mPosY = SCREEN_HEIGHT - lPaddle.getHeight();

    if (lPaddle.mPosX < 0)
        lPaddle.mPosX = 0;
    if (lPaddle.mPosX + lPaddle.getWidth() > SCREEN_WIDTH/2)
        lPaddle.mPosX = SCREEN_WIDTH/2 - lPaddle.getWidth();

    if (rPaddle.mPosY < 0) 
        rPaddle.mPosY = 0;
    if (rPaddle.mPosY + rPaddle.getHeight() > SCREEN_HEIGHT)
        rPaddle.mPosY = SCREEN_HEIGHT - rPaddle.getHeight();

    if (rPaddle.mPosX < SCREEN_WIDTH/2)
        rPaddle.mPosX = SCREEN_WIDTH/2;
    if (rPaddle.mPosX + rPaddle.getWidth() > SCREEN_WIDTH)
        rPaddle.mPosX = SCREEN_WIDTH - rPaddle.getWidth();

}

void input(){
    SDL_Event e;
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    while (SDL_PollEvent(&e)){
        if (e.type==SDL_QUIT) isRunning = false;
        if (playButton.handleButton(&e)) gamemode = 10;
        if (paddlex1.handleButton(&e)) gamemode = 21;
        if (paddlex2.handleButton(&e)) gamemode = 22;
    }
        

   

    if (keystates[SDL_SCANCODE_UP]) rPaddle.mPosY -=  rPaddle.PAD_SPEED;
    if (keystates[SDL_SCANCODE_DOWN]) rPaddle.mPosY += rPaddle.PAD_SPEED;
    if (keystates[SDL_SCANCODE_LEFT]) rPaddle.mPosX -= rPaddle.PAD_SPEED;
    if (keystates[SDL_SCANCODE_RIGHT]) rPaddle.mPosX += rPaddle.PAD_SPEED;

    if (keystates[SDL_SCANCODE_W]) lPaddle.mPosY -=  lPaddle.PAD_SPEED;
    if (keystates[SDL_SCANCODE_S]) lPaddle.mPosY += lPaddle.PAD_SPEED;
    if (keystates[SDL_SCANCODE_A]) lPaddle.mPosX -= lPaddle.PAD_SPEED;
    if (keystates[SDL_SCANCODE_D]) lPaddle.mPosX += lPaddle.PAD_SPEED;
    
    if (keystates[SDL_SCANCODE_SPACE]) startBall = true;
    if (keystates[SDL_SCANCODE_ESCAPE]) opt = true;
}
void renderGameLayout(int mode){
    if (mode>0){
        gBackground.render();
        SDL_RenderFillRect(gRenderer, &topBar);
        SDL_RenderFillRect(gRenderer, &topLeftBar);
        SDL_RenderFillRect(gRenderer, &topRightBar);

        SDL_RenderFillRect(gRenderer, &botBar);
        SDL_RenderFillRect(gRenderer, &botLeftBar);
        SDL_RenderFillRect(gRenderer, &botRightBar);

        SDL_RenderFillRect(gRenderer, &topLeftWall);
        SDL_RenderFillRect(gRenderer, &botLeftWall);
        SDL_RenderFillRect(gRenderer, &topRightWall);
        SDL_RenderFillRect(gRenderer, &botRightWall);

        //render paddle
        lPaddle.render();
        rPaddle.render();

        //render ball
        ball.render();

        write(score, SCREEN_WIDTH/2 + FONT_SIZE, FONT_SIZE*2);
    }
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
    SDL_SetRenderDrawColor( gRenderer, 142, 177, 92, 0xFF );

    // gOpt.render();
    // playButton.render();
    // paddlex1.render();
    // paddlex2.render();
    gamemode = 1;
    //render game background
    renderGameLayout(gamemode);

    //render option board
    if (opt == true){
        gOpt.render();
        playButton.render();
        paddlex1.render();
        paddlex2.render();
    }
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

    int cordX = (SCREEN_WIDTH/2 -playButton.getWidth())/2;
    playButton.mPosX = cordX;
    paddlex1.mPosX = paddlex2.mPosX = SCREEN_WIDTH/2 + cordX;
    playButton.mPosY = paddlex1.mPosY = 409;
    paddlex2.mPosY = 587;


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