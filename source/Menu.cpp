#include "Menu.h"

#include <SDL.h>
#include <SDL_image.h>
#include <hal/debug.h>
#include <hal/video.h>
#include <hal/xbox.h>
#include <windows.h>
#include <xboxkrnl/xboxkrnl.h>

#include "common.h"
#include "i2c_map.h"

extern SDL_Renderer *gRenderer;
extern TTF_Font *gFontSmall;
extern uint8_t load_scene;
extern bool running;

Menu::Menu() {
  background_texture =
      loadTexture(gRenderer, "D:\\assets\\images\\menu_background.png");
  arrow_texture = loadTexture(gRenderer, "D:\\assets\\images\\arrow.png");

  current_item = 0;

  // Attempt to recieve firmware version
  ULONG smbus_read;

  //
  if(HalReadSMBusValue(I2C_HDMI_ADRESS, I2C_FIRMWARE_VERSION + 0, false, &smbus_read) != 0) {
    snprintf(text_buffer, sizeof(text_buffer), "XboxHDMI not detected!");
  } else {
    firmware_version[0] = (uint8_t)smbus_read;

    HalReadSMBusValue(I2C_HDMI_ADRESS, I2C_FIRMWARE_VERSION + 1, false, &smbus_read);
    firmware_version[1] = (uint8_t)smbus_read;

    HalReadSMBusValue(I2C_HDMI_ADRESS, I2C_FIRMWARE_VERSION + 2, false, &smbus_read);
    firmware_version[2] = (uint8_t)smbus_read;

    snprintf(text_buffer, sizeof(text_buffer), "Firmware Version: %u.%u.%u",
      firmware_version[0], firmware_version[1], firmware_version[2]);
  }

  info_line[0] = drawText(gFontSmall, font_color, text_buffer);
  SDL_QueryTexture(info_line[0], NULL, NULL,
                   &info_line_pos[0].w,
                   &info_line_pos[0].h);

  //
  if(findKernelPatchVersion(kernel_patch_version)) {
    snprintf(text_buffer, sizeof(text_buffer), "Kernel Patch Version: %u.%u.%u",
      kernel_patch_version[0], kernel_patch_version[1], kernel_patch_version[2]);
  } else {
    snprintf(text_buffer, sizeof(text_buffer), "Kernel patch not detected!");
  }

  info_line[1] = drawText(gFontSmall, font_color, text_buffer);
  SDL_QueryTexture(info_line[1], NULL, NULL,
                   &info_line_pos[1].w,
                   &info_line_pos[1].h);
}

Menu::~Menu(void) {
  SDL_DestroyTexture(background_texture);
  SDL_DestroyTexture(arrow_texture);
  SDL_DestroyTexture(info_line[0]);
}

void Menu::event(SDL_Event event) {
  int8_t button_press = proccess_event(event);

  ULONG temp_read = 0;

  switch (button_press) {
    case SDL_CONTROLLER_BUTTON_A:
      load_scene = current_item + 1;
      break;
    case SDL_CONTROLLER_BUTTON_B:
      running = false;
      break;
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
      current_item = (current_item - 1) % 3;
      break;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
      current_item = (current_item + 1) % 3;
      break;
    default:
      break;
  }
}

bool Menu::findKernelPatchVersion(uint8_t *version) {
  char tag[] = "HDMIkv";

  for(uint16_t offset = 0; offset < 0x00001000; offset++) {
    if(memcmp(tag, ((char *)&AvSetDisplayMode) + offset, sizeof(tag) - 1) == 0) {
      version[0] = (((char *)&AvSetDisplayMode) + offset)[6];
      version[1] = (((char *)&AvSetDisplayMode) + offset)[7];
      version[2] = (((char *)&AvSetDisplayMode) + offset)[8];

      return true;
    }
  }

  return false;
}

void Menu::render(SDL_Renderer *renderer) {
  SDL_RenderCopy(gRenderer, background_texture, NULL, NULL);
  SDL_RenderCopy(gRenderer, arrow_texture, NULL, &arrow_pos[current_item]);
  SDL_RenderCopy(gRenderer, info_line[0], NULL, &info_line_pos[0]);
  SDL_RenderCopy(gRenderer, info_line[1], NULL, &info_line_pos[1]);
}
