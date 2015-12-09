#include <stdio.h>
#include <unistd.h>
#include <SDL.h>
#include <stdint.h>
#include <stdbool.h>

#define max(x,y) ((x) > (y) ? (x) : (y))

#define global
global unsigned int Width = 512;
global unsigned int Height = 512;

struct my_box {
  int X;
  int Y;
  int Width;
  int Height;
};

global struct my_box JoyCursor;
void
PutPixel(uint32_t *DisplayBuffer,
         unsigned int Width,
         unsigned int Height,
         unsigned int Pitch,
         unsigned int X,
         unsigned int Y,
         uint32_t Color);

int main(int ArgC, char **ArgV) {
  JoyCursor.X = Width / 2.0;
  JoyCursor.Y = Height / 2.0;
  JoyCursor.Width = 16;
  JoyCursor.Height = 16;

  SDL_Joystick *Joystick;

  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) != 0) {
    fprintf(stderr, "\nUnable to initialize SDL: %s\n", SDL_GetError());
    return 1;
  }

  if (SDL_NumJoysticks() > 0) {
    Joystick = SDL_JoystickOpen(0);

    if (Joystick) {
      printf("Opened Joystick 0\n");
      printf("Name: %s\n", SDL_JoystickNameForIndex(0));
      printf("Number of Axes: %d\n", SDL_JoystickNumAxes(Joystick));
      printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(Joystick));
      printf("Number of Balls: %d\n", SDL_JoystickNumBalls(Joystick));
    } else {
      printf("Couldn't open Joystick 0\n");
    }
  }

  SDL_Window *Window;
  SDL_Renderer *Renderer;
  SDL_Texture *Texture;
  uint32_t *DisplayBuffer;
  int DisplayBufferPitch;

  Window = SDL_CreateWindow("Font Rendering Window", 100, 100, Width, Height, 0);
  if (Window == NULL) {
    fprintf(stderr, "\nCouldn't create window: %s\n", SDL_GetError());
    return 1;
  }

  Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_SOFTWARE);
  if (Renderer == NULL) {
    fprintf(stderr, "\nCouldn't create renderer: %s\n", SDL_GetError());
    return 1;
  }

  Texture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, Width, Height);
  if (Texture == NULL) {
    fprintf(stderr, "\nCouldn't create texture: %s\n", SDL_GetError());
    return 1;
  }

  bool Running = true;
  while(Running) {
    SDL_Event Event;

    while(SDL_PollEvent(&Event)) {
      switch(Event.type) {
        case SDL_QUIT: {
          Running = false;
          break;
        }

        case SDL_KEYDOWN:
        case SDL_KEYUP: {
          if (Event.key.repeat == 0 && Event.key.keysym.sym == SDLK_ESCAPE) {
            Running = false;
          }
          break;
        }
      }
    }

    int16_t JoyX = SDL_JoystickGetAxis(Joystick, 0);
    int16_t JoyY = SDL_JoystickGetAxis(Joystick, 1);

    JoyCursor.X = (JoyX == 0 ? 0 : (float)JoyX / (float)INT16_MAX) * Width + ((float)Width/2.0);
    JoyCursor.Y = (JoyY == 0 ? 0 : (float)JoyY / (float)INT16_MAX) * Height + ((float)Height/2.0);

    if (JoyCursor.X < 0 + JoyCursor.Width) { JoyCursor.X = 0 + JoyCursor.Width; }
    if (JoyCursor.X > Width - JoyCursor.Width) { JoyCursor.X = Width - JoyCursor.Width; }
    if (JoyCursor.Y < 0 + JoyCursor.Height) { JoyCursor.Y = 0 + JoyCursor.Height; }
    if (JoyCursor.Y > Height - JoyCursor.Height) { JoyCursor.Y = Height - JoyCursor.Height; }

    SDL_LockTexture(Texture, NULL, (void**)&DisplayBuffer, &DisplayBufferPitch);
    memset(DisplayBuffer, 0, Height * DisplayBufferPitch); // Zero out the entire texture

    unsigned int HalfWidth = JoyCursor.Width / 2.0;
    unsigned int HalfHeight = JoyCursor.Height / 2.0;
    for (int Y = 0; Y < JoyCursor.Height; ++Y) {
      for (int X = 0; X < JoyCursor.Width; ++X) {
        unsigned int XPos = JoyCursor.X - HalfWidth + X;
        unsigned int YPos = JoyCursor.Y - HalfHeight + Y;
        PutPixel(DisplayBuffer, Width, Height, DisplayBufferPitch, XPos, YPos, 0xFFFFFFFF);
      }
    }

    SDL_UnlockTexture(Texture);
    SDL_RenderClear(Renderer);
    SDL_RenderCopy(Renderer, Texture, 0, 0);
    SDL_RenderPresent(Renderer);
  }

  SDL_DestroyTexture(Texture);
  SDL_DestroyRenderer(Renderer);
  SDL_DestroyWindow(Window);

  if (SDL_JoystickGetAttached(Joystick)) {
    SDL_JoystickClose(Joystick);
  }

  SDL_Quit();

  return 0;
}

void
PutPixel(uint32_t *DisplayBuffer,
         unsigned int Width,
         unsigned int Height,
         unsigned int Pitch,
         unsigned int X,
         unsigned int Y,
         uint32_t Color) {

  uint32_t *DestRow = (uint32_t *)(((uint8_t*)DisplayBuffer) + (Pitch * Y));

  *(DestRow + X) = Color;
}
