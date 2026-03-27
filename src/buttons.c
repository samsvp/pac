#include "buttons.h"

buttons_t button_from_char(char c) {
  buttons_t button = BUTTON_NONE;

  switch (c) {
    case 'u': button = BUTTON_UP; break;
    case 'd': button = BUTTON_DOWN; break;
    case 'l': button = BUTTON_LEFT; break;
    case 'r': button = BUTTON_RIGHT; break;
    case 'c': button = BUTTON_COIN; break;
    default: break;
  }

  return button;
}
