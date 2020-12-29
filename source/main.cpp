#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef _XBOX
#include <SDL.h>
#include <SDL_image.h>
#include <hal/debug.h>
#include <hal/video.h>
#include <hal/xbox.h>
#include <windows.h>
#define FONT_PATH "D:\\assets\\fonts\\RobotoMono-Regular.ttf"
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#define FONT_PATH "assets/fonts/RobotoMono-Regular.ttf"
#endif

#include "common.h"
#include "scene.h"
#include "Menu.h"
#include "SelfTest.h"
#include "Advanced.h"
#include "FirmwareUpdate.h"

#define MENU 0
#define SELFTEST 1
#define ADVANCED 2
#define FIRMWARE_UPDATE 3

void criticalErrorAlert(char *msg, char *error) {
  debugPrint("%u: %u\n", msg, error);
  debugPrint("Rebooting in 5 seconds.\n");
  Sleep(5000);
  XReboot();
}

#ifdef _XBOX
// Screen dimension constants
// const extern int SCREEN_WIDTH;
// const extern int SCREEN_HEIGHT;
int SCREEN_WIDTH = 720;
int SCREEN_HEIGHT = 480;
#else
int SCREEN_WIDTH = 720;
int SCREEN_HEIGHT = 480;
#endif

SDL_Renderer *gRenderer = NULL;

TTF_Font *gFont = NULL;
TTF_Font *gFontSmall = NULL;
SDL_Window *window = NULL;

uint8_t current_scene = 0;
uint8_t load_scene = 0;

bool running = true;

int main(void) {
#ifdef _XBOX
  XVideoSetMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, REFRESH_DEFAULT);
#endif

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
    debugPrint("SDL_VideoInit failed: %s", SDL_GetError());
    Sleep(2000);
    return 0;
  }

  // Load joystick
  SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

  if (SDL_NumJoysticks() < 1) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Couldn't find any joysticks.\n");
    criticalErrorAlert("SDL_Error ", SDL_GetError);
  } else {
    SDL_GameController *controller = NULL;
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
      if (SDL_IsGameController(i)) {
        controller = SDL_GameControllerOpen(i);
        if (controller == nullptr) {
        criticalErrorAlert("SDL_Error", SDL_GetError);
        }
      }
    }
  }

  if (TTF_Init() != 0) {
    debugPrint("TTF_Init failed: %s", TTF_GetError());
    Sleep(2000);
    return 0;
  }

  window = SDL_CreateWindow("XboxHDMI", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                            SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    debugPrint("Window creation failed: %s", SDL_GetError());
    Sleep(2000);
    return 0;
  }

  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Couldn't intialize SDL_image.\n");
    SDL_VideoQuit();
    criticalErrorAlert("SDL_Image Error", IMG_GetError);
  }

  gRenderer = SDL_CreateRenderer(window, -1, 0);
  if (gRenderer == NULL) {
    debugPrint("CreateRenderer failed: %s\n", SDL_GetError());
    Sleep(2000);
    return 0;
  }

// Load font
  gFont = TTF_OpenFont(FONT_PATH, 26);
  if (gFont == NULL) {
    debugPrint("Couldn't load font: %s", TTF_GetError());
    Sleep(2000);
    return 0;
  }
  gFontSmall = TTF_OpenFont(FONT_PATH, 20);
  if (gFontSmall == NULL) {
    debugPrint("Couldn't load font: %s", TTF_GetError());
    Sleep(2000);
    return 0;
  }

  // Main render loop
  Scene *currentScene = new Menu();

  while (running) {
#ifdef _XBOX
    XVideoWaitForVBlank();
#endif

    if (load_scene != current_scene) {
      delete currentScene;
      current_scene = load_scene;
      switch (load_scene) {
        case MENU:
          currentScene = new Menu();
          break;
        case SELFTEST:
          currentScene = new SelfTest();
          break;
        case ADVANCED:
          currentScene = new Advanced();
          break;
        case FIRMWARE_UPDATE:
          currentScene = new FirmwareUpdate();
          break;  
        default:
          load_scene = 0;
          break;
      }
    }

    // Check for events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
          running = false;
      }

      currentScene->event(event);
    }

    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(gRenderer);

    currentScene->render(gRenderer);

    SDL_RenderPresent(gRenderer);
  }

  return 0;
}
