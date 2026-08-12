/*
===========================================================================
Name        : DemoApps.h
Author      : Brandon Van Pelt
Description : Two small example apps for the EmbeddedGFX demo, written the same
              way any project app is: a createBtns() that lays out buttons and
              a handler() that reacts to taps.
===========================================================================
*/
#ifndef DEMOAPPS_H
#define DEMOAPPS_H

#include "appConfig.h"

// About page — a static informational screen.
uint8_t about_createBtns(void);
void    about_handler(int userInput);

// Counter page — +/- buttons that update a live value.
uint8_t counter_createBtns(void);
void    counter_handler(int userInput);

#endif // DEMOAPPS_H
