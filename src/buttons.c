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

button_event_t button_event_from_char(char c) {
  button_event_t event = EVENT_NONE;

  switch (c) {
    case 'u': event = EVENT_UP; break;
    case 'd': event = EVENT_DOWN; break;
    default: break;
  }

  return event;
}
