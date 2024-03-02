#include <iostream>
#include <windows.h>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class LTexture
{
public:
    LTexture();
    ~LTexture();

    bool loadFromFile (std::string path);
    int getWidth();
    int getHeight();
    void render(int x, int y);
    void free();

private:
    SDL_Texture* mTexture;
    int mWidth;
    int mHeight;
};