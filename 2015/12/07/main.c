#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <SDL.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

typedef int bool;
#define false 0
#define true !false

#define global
typedef unsigned char u8;
typedef unsigned int u32;

#define Width 512
#define Height 512

int main(int ArgC, char **ArgV) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "\nUnable to initialize SDL: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window *Window;
  SDL_Renderer *Renderer;
  SDL_Texture *Texture;

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

  unsigned char TtfBuffer[1<<20];
  unsigned char MonoBitmap[Width * Height];
  stbtt_bakedchar CharData[96]; // ASCII 32..126 is 95 glyphs
  char FontFilePath[255] = {0};

  if (0 == getcwd(FontFilePath, 255)) {
    printf("Encountered an error calling getcwd\n");
    return 1;
  }
  char *Filename = "/ClearSans-Regular.ttf";
  snprintf(FontFilePath + strlen(FontFilePath), strlen(Filename) + 1, "%s", Filename);

  fread(TtfBuffer, 1, 1<<20, fopen(FontFilePath, "rb"));
  stbtt_BakeFontBitmap(TtfBuffer,0, 32.0, MonoBitmap,Width,Height, 32,96, CharData); // no guarantee this fits!

  unsigned int *DisplayBuffer;
  int DisplayBufferPitch;
  SDL_LockTexture(Texture, NULL, (void**)&DisplayBuffer, &DisplayBufferPitch);

  unsigned char *Source = (unsigned char *)MonoBitmap;
  unsigned char *DestRow = (unsigned char *)DisplayBuffer;
  for (int Y = 0; Y < Height; ++Y) {
    unsigned int *DisplayBufferPixel = (unsigned int *)DestRow;
    for (int X = 0; X < Width; ++X) {
      unsigned char Alpha = *Source++;
      *DisplayBufferPixel++ = ((Alpha << 24) |
                               (Alpha << 16) |
                               (Alpha <<  8) |
                               (Alpha <<  0));
    }
    DestRow += DisplayBufferPitch;
  }

  SDL_UnlockTexture(Texture);
  SDL_RenderClear(Renderer);
  SDL_RenderCopy(Renderer, Texture, 0, 0);
  SDL_RenderPresent(Renderer);

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
  }

  //stbtt_FreeBitmap(MonoBitmap, 0);
  SDL_DestroyTexture(Texture);
  SDL_DestroyRenderer(Renderer);
  SDL_DestroyWindow(Window);
  SDL_Quit();

  return 0;
}
