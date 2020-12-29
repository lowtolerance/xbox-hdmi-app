#ifndef SCENE0_H_
#define SCENE0_H_

#if _XBOX
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "scene.h"

class Menu : public Scene {
 public:
  Menu();
  virtual ~Menu();
  void event(SDL_Event event);
  void render(SDL_Renderer *renderer);

 private:
  bool findKernelPatchVersion(uint8_t *version);

  uint8_t current_item = 0;

  SDL_Texture *background_texture;
  SDL_Texture *arrow_texture;
  const SDL_Color font_color = {0xFF, 0xFF, 0xFF, 0xFF};

  SDL_Texture *info_line[2] = { NULL };
  SDL_Rect info_line_pos[2] = {
      {80, 420, 0, 0},  // Firmware Version
      {80, 396, 0, 0},  // BIOS Patch Version
  };

  const SDL_Rect arrow_pos[4] = {
      {406, 190 + (40 * 0), 24, 16},  // Self Test
      {406, 190 + (40 * 1), 24, 16},  // Advanced
      {406, 190 + (40 * 2), 24, 16},  // Firmware Update
  };

  uint8_t firmware_version[3] = { 0 };
  uint8_t kernel_patch_version[3] = { 0 };

  char text_buffer[100] = { 0 };
};

#endif
