#include "SDL.h"
#include "controller.h"
SDL_Joystick *jstick;
int joystick_init(int id);
int joystick_update(packet_t *ctl);
void joystick_release();
