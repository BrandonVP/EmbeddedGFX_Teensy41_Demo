/*
===========================================================================
Name        : appConfig.h
Author      : Brandon Van Pelt
Description : Project-specific configuration for the EmbeddedGFX demo.

              This is where the PROJECT (not the library) defines its screen
              size overrides and its own menu / app id enums. Include this file
              instead of <EmbeddedGFX.h> directly, so the GFX_* overrides are
              always seen before the library headers.
===========================================================================
*/
#ifndef APPCONFIG_H
#define APPCONFIG_H

// --- Library configuration overrides (must precede <EmbeddedGFX.h>) --------
#define GFX_SCREEN_WIDTH     480
#define GFX_SCREEN_HEIGHT    320
#define GFX_APP_BUTTON_SIZE  30
#define GFX_MENU_BUTTON_SIZE 3

#include <EmbeddedGFX.h>

// --- Project menus ---------------------------------------------------------
// The three top-of-screen menus. Fully under project control — reorder, rename,
// add or remove as desired.
enum Menus {
    MENU_main = 0,
    MENU_tools,
    MENU_settings
};

// --- Project apps ----------------------------------------------------------
// Each id is a slot in the framework's app registry. The three *_MENU entries
// are the generic "list of apps on this menu" pages; the rest are real apps.
enum AppLabels {
    APP_HOME_MENU = 0,   // main menu landing page
    APP_TOOLS_MENU,      // tools menu landing page
    APP_SETTINGS_MENU,   // settings menu landing page
    APP_ABOUT,           // main   > About
    APP_COUNTER,         // tools  > Counter
    APP_THEME,           // settings > Themes
    APP_COUNT
};

#endif // APPCONFIG_H
