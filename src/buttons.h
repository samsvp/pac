#ifndef PAC_BUTTONS_H
#define PAC_BUTTONS_H

/**
 * Controller for the socket connection.
 */

typedef enum {
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_LEFT,
  BUTTON_RIGHT,
  BUTTON_COIN,
  BUTTON_START,
  BUTTON_NONE,
} buttons_t;

typedef enum {
  EVENT_DOWN,
  EVENT_UP,
  EVENT_NONE,
} button_event_t;

buttons_t button_from_char(char c);
button_event_t button_event_from_char(char c);

#endif
