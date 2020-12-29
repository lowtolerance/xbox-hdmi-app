// #ifndef SCENE4_H_
// #define SCENE4_H_

#include <SDL.h>

#include "scene.h"

class Credits : public Scene {
 public:
  Scene4();
  virtual ~Credits();
  void event(SDL_Event event);
  void render(SDL_Renderer* renderer);

 private:
  SDL_Texture* background_texture;
};

#endif
