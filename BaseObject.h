#ifndef BASE_OBJECT_H_
#define BASE_OBJECT_H_

#include <iostream>
#include <windows.h>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
// #include <SDL2/SDL_mixer.h>
// #include <SDL2/SDL_tff.h>

class BaseObject
{
public:
    BaseObject();
    ~BaseObject();

    void setRect(const int& x, const int& y) 
    {
        rect.x = x;
        rect.y = y;
    }

    SDL_Rect GetRect() const {
        return rect;
    }

    SDL_Texture* GetObject() const{
        return p_object;
    }

    bool LoadImg(std::string path, SDL_Renderer* screen);
    void Render(SDL_Renderer* des, const SDL_Rect* clip = NULL);
    void Free();

protected:
    SDL_Texture* p_object;
    SDL_Rect rect;
};

#endif