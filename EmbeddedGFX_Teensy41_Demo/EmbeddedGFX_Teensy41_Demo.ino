/*
 ===========================================================================
 Name        : EmbeddedGFX_Teensy41_Demo.ino
 Author      : Brandon Van Pelt
 Description : Demo app for the EmbeddedGFX library on Teensy 4.1 with an
               ILI9488 display and FT6206 capacitive touch.

               Shows the full pattern for using the library from a project:
                 1. Provide display/touch adapters (ILI9488Adapter/FT6206Adapter)
                 2. Own the button arrays
                 3. Register apps in setup()
                 4. Pump touch + app.run() in loop()

               Hardware wiring mirrors ScanToolFD (TFT_CS=10, TFT_DC=9, BL=14).
 ===========================================================================
 */

#include <SPI.h>
#include <ILI9488_t3.h>
#include <Adafruit_FT6206.h>
#include <TimeLib.h>            // clock in the status bar (Teensy RTC)

#include "appConfig.h"          // GFX overrides + <EmbeddedGFX.h> + project enums
#include <apps/ThemeApp.h>      // optional theme-picker app from the library
#include "ILI9488Adapter.h"
#include "FT6206Adapter.h"
#include "DemoApps.h"
#include "font_Michroma.h"

// --- Pins (mirror ScanToolFD) ----------------------------------------------
#define TFT_CS 10
#define TFT_DC  9
#define LCD_BL 14

// --- Hardware drivers ------------------------------------------------------
ILI9488_t3      tft = ILI9488_t3(&SPI, TFT_CS, TFT_DC);
Adafruit_FT6206 ts  = Adafruit_FT6206();

// --- EmbeddedGFX adapters + registry ---------------------------------------
ILI9488Adapter gfxDisplay(tft);
FT6206Adapter  gfxTouch(ts);
App app;

// Project owns the button arrays (DMAMEM keeps them in RAM2 on Teensy).
DMAMEM UserInterfaceClass appButtons[GFX_APP_BUTTON_SIZE];
UserInterfaceClass        menuButtons[GFX_MENU_BUTTON_SIZE];

// --- Status bar (clock left, battery right) --------------------------------
// The menu bar is 0..45 tall. We reserve the left ~78px for the clock and the
// right ~45px for a battery icon, leaving a center band for the menu buttons.
#define STATUS_CLOCK_W   78
#define STATUS_BATT_W    45

time_t getTeensy3Time() { return Teensy3Clock.get(); }

// Draw HH:MM:SS over MM/DD/YY at the top left, in the current buffer mode.
void drawClock()
{
    char t[16], d[16];
    snprintf(t, sizeof(t), "%02d:%02d:%02d", hour(), minute(), second());
    snprintf(d, sizeof(d), "%02d/%02d/%02d", month(), day(), year() % 100);

    tft.setTextColor(gfxTheme.btnTextColor);
    tft.setFont(Michroma_8);
    tft.fillRect(2, 6, STATUS_CLOCK_W, 32, gfxTheme.menuBg);
    tft.drawString(t, strlen(t), 4, 8);
    tft.drawString(d, strlen(d), 4, 24);
    tft.setFont(Michroma_11);
}

// Draw a battery glyph with a proportional fill at the top right.
void drawBattery()
{
    // Demo value — a real project substitutes its measured battery level here.
    const int pct = 82;

    const int bx = GFX_SCREEN_WIDTH - STATUS_BATT_W + 3; // body left
    const int by = 14, bw = 30, bh = 16;

    tft.fillRect(bx + bw, by + 4, 3, 8, gfxTheme.btnTextColor);   // terminal nub
    tft.drawRect(bx, by, bw, bh, gfxTheme.btnTextColor);          // outline
    tft.fillRect(bx + 2, by + 2, bw - 4, bh - 4, gfxTheme.menuBg); // clear interior

    int fillW = ((bw - 4) * pct) / 100;
    uint16_t c = (pct > 50) ? 0x07E0 : (pct > 20 ? 0xFFE0 : 0xF800); // green/yellow/red
    tft.fillRect(bx + 2, by + 2, fillW, bh - 4, c);
}

// Refresh the status bar once a second. Drawn to both the live panel and the
// frame buffer so a later updateScreen() doesn't erase it.
void updateStatusBar()
{
    static uint32_t last = 0;
    if (millis() - last < 1000) return;
    last = millis();

    tft.useFrameBuffer(false);
    drawClock();
    drawBattery();
    tft.useFrameBuffer(true);
    drawClock();
    drawBattery();
}

// --- Menu bar --------------------------------------------------------------
void createMenuBtns()
{
    // Center band between the clock (left) and battery (right).
    menuButtons[0].setButton( 82, 0, 192, 45, APP_HOME_MENU,     true, 0, "Home",     ALIGN_CENTER, gfxTheme.menuBg, gfxTheme.menuBg, gfxTheme.btnTextColor);
    menuButtons[1].setButton(197, 0, 307, 45, APP_TOOLS_MENU,    true, 0, "Tools",    ALIGN_CENTER, gfxTheme.menuBg, gfxTheme.menuBg, gfxTheme.btnTextColor);
    menuButtons[2].setButton(312, 0, 422, 45, APP_SETTINGS_MENU, true, 0, "Settings", ALIGN_CENTER, gfxTheme.menuBg, gfxTheme.menuBg, gfxTheme.btnTextColor);
}

// Draw the top menu bar. Also used as ThemeApp's menu-redraw hook.
void drawMenuBar()
{
    GUI_I.drawSquareBtn(0,  0, GFX_SCREEN_WIDTH, 45, "", gfxTheme.menuBg, gfxTheme.menuBg, gfxTheme.menuBg, ALIGN_CENTER);
    GUI_I.drawSquareBtn(0, 45, GFX_SCREEN_WIDTH, GFX_MENU_BAR_HEIGHT, "", gfxTheme.menuBorder, gfxTheme.menuBorder, gfxTheme.menuBorder, ALIGN_CENTER);

    createMenuBtns();
    uint8_t state = 0;
    while (GUI_I.drawPage(menuButtons, state, GFX_MENU_BUTTON_SIZE));
    GUI_I.setGraphicLoaderState(0);

    // Underline the active app's menu button. buttonMonitor does this on tap;
    // draw it here so the default page is underlined on load (and after a theme
    // redraw) too. Menu ids match menuButtons order (main=0, tools=1, settings=2).
    gfx_menu_id_t activeMenu = app.getActiveMenu();
    if (activeMenu < GFX_MENU_BUTTON_SIZE)
    {
        UserInterfaceClass& mb = menuButtons[activeMenu];
        GUI_I.drawSquareBtn(mb.getXStart(), 45, mb.getXStop(), 50, "", gfxTheme.btnColor, gfxTheme.btnColor, mb.getBorderColor(), ALIGN_CENTER);
    }

    drawClock();
    drawBattery();
    tft.updateScreen();
}

// --- App registration ------------------------------------------------------
void registerApps()
{
    // Menu landing pages (generic behaviour supplied by the library).
    app.add(MENU_main,     "Home",     APP_HOME_MENU,     GFX_menuInput, GFX_createMenu);
    app.add(MENU_tools,    "Tools",    APP_TOOLS_MENU,    GFX_menuInput, GFX_createMenu);
    app.add(MENU_settings, "Settings", APP_SETTINGS_MENU, GFX_menuInput, GFX_createMenu);

    // Real apps, each assigned to a menu.
    app.add(MENU_main,     "About",    APP_ABOUT,   about_handler,    about_createBtns);
    app.add(MENU_tools,    "Counter",  APP_COUNTER, counter_handler,  counter_createBtns);
    app.add(MENU_settings, "Themes",   APP_THEME,   ThemeApp_handler, ThemeApp_createBtns);
}

// ---------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);

    setSyncProvider(getTeensy3Time);   // status-bar clock from the Teensy RTC

    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, HIGH);

    ts.begin(40);
    tft.begin();
    tft.useFrameBuffer(true);
    tft.fillScreen(ILI9488_BLACK);
    tft.setRotation(1);
    tft.setFont(Michroma_11);

    // Wire the framework to the hardware and the button arrays.
    GUI_I.begin(gfxDisplay, gfxTouch, appButtons, menuButtons);
    GUI_I.setApp(&app);

    // Themes: RAM-only for the demo (no persistence). Repaint the menu bar when
    // the palette changes.
    ThemeApp_setMenuRedraw(drawMenuBar);
    ThemeApp_begin();

    registerApps();
    app.init();

    drawMenuBar();
}

// ---------------------------------------------------------------------------
void loop()
{
    GUI_I.buttonMonitor(menuButtons, GFX_MENU_BUTTON_SIZE);
    GUI_I.updateTouch();
    app.run();
    updateStatusBar();
}
