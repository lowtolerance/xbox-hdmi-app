#include "Credits.h"

#include <SDL.h>

#include "common.h"

extern SDL_Renderer *gRenderer;

extern uint8_t load_scene;

Credits::Credits() {
  background_texture =
      loadTexture(gRenderer, "D:\\assets\\images\\credits_background.png");
}

Credits::~Credits(void) { SDL_DestroyTexture(background_texture); }

void Credits::event(SDL_Event event) {
  switch (event.type) {
    case SDL_JOYBUTTONDOWN:
      // B Button - Go back to main menu
      if (event.jbutton.button == 1) {
        load_scene = 0;
      }
      break;
  }
}

void Credits::render(SDL_Renderer *renderer) {
  SDL_RenderCopy(gRenderer, background_texture, NULL, NULL);
}
