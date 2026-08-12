/*
===========================================================================
Name        : DemoApps.cpp
Author      : Brandon Van Pelt
Description : Example apps for the EmbeddedGFX demo (see DemoApps.h).
===========================================================================
*/
#include "DemoApps.h"

// ---------------------------------------------------------------------------
// About page
// ---------------------------------------------------------------------------
uint8_t about_createBtns(void)
{
    UserInterfaceClass* b = GUI_I.appButtons();

    b[0].setButton(40, 90, 440, 150, 0, true, 12, "EmbeddedGFX", ALIGN_CENTER, gfxTheme.btnColor, gfxTheme.btnBorder, gfxTheme.btnColor, gfxTheme.btnText);
    b[0].setTextSize(24);
    b[0].setClickable(false);

    b[1].setButton(40, 165, 440, 210, 0, true, 12, "Teensy 4.1 Demo", ALIGN_CENTER, gfxTheme.background, gfxTheme.background, gfxTheme.background, gfxTheme.btnTextColor);
    b[1].setTextSize(14);
    b[1].setClickable(false);

    b[2].setButton(40, 215, 440, 255, 0, true, 12, "v0.1.0", ALIGN_CENTER, gfxTheme.background, gfxTheme.background, gfxTheme.background, gfxTheme.btnTextColor);
    b[2].setTextSize(12);
    b[2].setClickable(false);

    return 3;
}

void about_handler(int userInput)
{
    (void)userInput; // static page, nothing to do
}

// ---------------------------------------------------------------------------
// Counter page
// ---------------------------------------------------------------------------
enum { CTR_MINUS = 1, CTR_PLUS = 2 };

static int   counterValue = 0;
static const uint8_t CTR_VALUE_BTN = 1; // index of the live value button

uint8_t counter_createBtns(void)
{
    UserInterfaceClass* b = GUI_I.appButtons();

    b[0].setButton(60, 130, 160, 230, CTR_MINUS, true, 16, "-", ALIGN_CENTER, gfxTheme.btnColor, gfxTheme.btnBorder, gfxTheme.orangeBtn, gfxTheme.btnText);
    b[0].setTextSize(40);

    b[CTR_VALUE_BTN].setButton(180, 130, 300, 230, 0, true, 12, "", ALIGN_CENTER, gfxTheme.background, gfxTheme.btnBorder, gfxTheme.background, gfxTheme.btnTextColor);
    b[CTR_VALUE_BTN].setTextInt(counterValue);
    b[CTR_VALUE_BTN].setTextSize(40);
    b[CTR_VALUE_BTN].setClickable(false);

    b[2].setButton(320, 130, 420, 230, CTR_PLUS, true, 16, "+", ALIGN_CENTER, gfxTheme.btnColor, gfxTheme.btnBorder, gfxTheme.orangeBtn, gfxTheme.btnText);
    b[2].setTextSize(40);

    return 3;
}

void counter_handler(int userInput)
{
    if (userInput == CTR_MINUS)
        counterValue--;
    else if (userInput == CTR_PLUS)
        counterValue++;
    else
        return;

    // Redraw just the value button with its new text, then push to the panel.
    GUI_I.appButtons()[CTR_VALUE_BTN].setTextInt(counterValue);
    GUI_I.updateButton(CTR_VALUE_BTN);
    GUI_I.updateScreen();
}
