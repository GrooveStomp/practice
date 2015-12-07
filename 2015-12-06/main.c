#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

Uint32 TimerCallback(Uint32 Interval, void *Param) {
  printf("TimerCallback is being executed\n");
  SDL_Event Event;
  SDL_UserEvent UserEvent;

  UserEvent.type = SDL_USEREVENT;
  UserEvent.code = 0;
  UserEvent.data1 = NULL;
  UserEvent.data2 = NULL;

  Event.type = SDL_USEREVENT;
  Event.user = UserEvent;

  SDL_PushEvent(&Event);

  return 0; /* Interval; */
}

int
main(int ArgC, char **ArgV) {
  Uint32 DelayMs = (333 / 10) * 10; /* 333 ms rounded down to the nearest 10 ms. */
  void *TimerCallbackParam;
  SDL_Event Event;

  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_AUDIO) != 0) {
    fprintf(stderr, "\nUnable to initialize SDL: %s\n", SDL_GetError());
    return 1;
  }

  atexit(SDL_Quit);

  if (SDL_WasInit(SDL_INIT_VIDEO) == 0) {
    printf("Video is not initialized.\n");
    return 1;
  }

  if (SDL_WasInit(SDL_INIT_TIMER) == 0) {
    printf("Timer is not initialized.\n");
    return 1;
  }

  if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {
    printf("Audio is not initialized.\n");
    return 1;
  }

  SDL_TimerID TimerID = SDL_AddTimer(DelayMs, TimerCallback, TimerCallbackParam);
  printf("Waiting for timer to expire...\n");

  if (SDL_WaitEvent(&Event)) {
    printf("Received Event\n");
    return 0;
  }
  else {
    printf("SDL_Waitevent failed: %s\n", SDL_GetError());
    return 1;
  }

  return 0;
}
