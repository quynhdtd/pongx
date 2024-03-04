#pragma once
#include <iostream>
#include <windows.h>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class LTexture
{
public:
    int mPosX, mPosY;
    float mVelX, mVelY;

    LTexture();
    ~LTexture();

    bool loadFromFile (std::string path);
    int getWidth();
    int getHeight();
    void render();
    void free();
    

private:
    SDL_Texture* mTexture;
    int mWidth, mHeight;
};

class Ball : public LTexture
{
public:
    int BALL_SIZE = 24;
    int BALL_SPEED = 19;
    SDL_Rect mCollider;
    void moveBall (SDL_Rect &wall);
};

class Paddle : public LTexture
{
public:
    int PAD_SPEED = 9;
    void LTBouncingBall(int tag);
    void limitPaddle();
    void targetPaddle();
};

class LButton : public LTexture
{
public:
	int BUTTON_WIDTH = 334;
    int BUTTON_HEIGHT = 102;
    int id;
    bool handleButton(SDL_Event* e);
};