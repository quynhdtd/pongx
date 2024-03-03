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
    // void render(int x, int y);
    void render();

    //take key press and change vel
    void handleEvent(SDL_Event &e);

    //move texture
    void move();
    void free();

private:
    SDL_Texture* mTexture;
    int mWidth, mHeight;
    
};

class Ball : public LTexture
{
public:
    int BALL_SIZE = 24;
    int BALL_SPEED = 24;
};

class Paddle : public LTexture
{
public:
    int PAD_SPEED = 9;
    //void movePaddle(int id);
};
