#include "SDL.h"
#include <stdio.h>

#define DISPLAY_WIDTH 640
#define DISPLAY_HEIGHT 480
#define RGBA_PIXEL_BYTE_SIZE 4
#define false 0
#define true !false
#define bool int

extern void BackendRenderTexture();

int main(int argc, char** argv) {
  SDL_Window* Window;
  SDL_Renderer* Renderer;
  SDL_Texture* Texture;
  int DisplayBufferPitch;

  int *DisplayBuffer = (int*)malloc(DISPLAY_WIDTH * DISPLAY_HEIGHT * RGBA_PIXEL_BYTE_SIZE);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
    fprintf(stderr, "\nUnable to initialize SDL: %s\n", SDL_GetError());
    return 1;
  }

  Window = SDL_CreateWindow("My Awesome Window", 100, 100, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0);
  if (Window == NULL) {
    fprintf(stderr, "\nCouldn't create window: %s\n", SDL_GetError());
    return 1;
  }

  Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_SOFTWARE);
  if (Renderer == NULL) {
    fprintf(stderr, "\nCouldn't create renderer: %s\n", SDL_GetError());
    return 1;
  }

  Texture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, DISPLAY_WIDTH, DISPLAY_HEIGHT);
  if (Texture == NULL) {
    fprintf(stderr, "\nCouldn't create texture: %s\n", SDL_GetError());
    return 1;
  }

  SDL_LockTexture(Texture, NULL, (void**)&DisplayBuffer, &DisplayBufferPitch);
  BackendRenderTexture(DisplayBuffer, DisplayBufferPitch, DISPLAY_WIDTH, DISPLAY_HEIGHT);
  SDL_UnlockTexture(Texture);

  bool Running = true;
  while(Running) {
    SDL_Event Event;

    while(SDL_PollEvent(&Event)) {
      switch(Event.type) {
        case SDL_QUIT: {
          Running = false;
        } break;

        case SDL_KEYDOWN:
        case SDL_KEYUP: {
          SDL_Keycode KeyCode = Event.key.keysym.sym;

          if(Event.key.repeat == 0) {
            if(KeyCode == SDLK_ESCAPE) {
              Running = false;
            }
          }
        } break;
      }
    }

    SDL_RenderClear(Renderer);
    SDL_RenderCopy(Renderer, Texture, 0, 0);
    SDL_RenderPresent(Renderer);
  }

  SDL_DestroyTexture(Texture);
  SDL_DestroyRenderer(Renderer);
  SDL_DestroyWindow(Window);
  SDL_Quit();

  return 0;
}
