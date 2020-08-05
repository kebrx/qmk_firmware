#include QMK_KEYBOARD_H

#include <print.h>
#include <string.h>
// HID has not yet been implemented for this keyboard
// #include "raw_hid.h"

#define MILLISECONDS_IN_SECOND 1000

// These are just to make it neater to use builtin HSV values in the keymap
#define RED {HSV_RED}
#define CORAL {HSV_CORAL}
#define ORANGE {HSV_ORANGE}
#define GOLDEN {HSV_GOLDENROD}
#define GOLD {HSV_GOLD}
#define YELLOW {HSV_YELLOW}
#define CHART {HSV_CHARTREUSE}
#define GREEN {HSV_GREEN}
#define SPRING {HSV_SPRINGGREEN}
#define TURQ {HSV_TURQUOISE}
#define TEAL {HSV_TEAL}
#define CYAN {HSV_CYAN}
#define AZURE {HSV_AZURE}
#define BLUE {HSV_BLUE}
#define PURPLE {HSV_PURPLE}
#define MAGENT {HSV_MAGENTA}
#define PINK {HSV_PINK}

// Spork Defined HSVs

// HSVs are 0-255 on each.
// Hue changes color, Saturation changes grayscale, value changes intensity.
#define S_PURPLE {190,255,200}
#define DARK {1,1,1}

//========================================================== CONFIGURABLE DEFAULTS ==========================================================
#define RGB_DEFAULT_TIME_OUT 120
#define RGB_FAST_MODE_TIME_OUT 3
#define RGB_TIME_OUT_MAX 600
#define RGB_TIME_OUT_MIN 10
#define RGB_TIME_OUT_STEP 10

extern bool g_suspend_state;
extern rgb_config_t rgb_matrix_config;
bool disable_layer_color;

bool rgb_enabled_flag;                  // Current LED state flag. If false then LED is off.
bool rgb_time_out_enable;               // Idle LED toggle enable. If false then LED will not turn off after idle timeout.
bool rgb_time_out_fast_mode_enabled;    // Enable flag for RGB timeout fast mode
bool rgb_time_out_user_value;           // This holds the toggle value set by user with ROUT_TG. It's necessary as RGB_TOG changes timeout enable.
uint16_t rgb_time_out_seconds;          // Idle LED timeout value, in seconds not milliseconds
uint16_t rgb_time_out_saved_seconds;    // The saved user config for RGB timeout period
led_flags_t rgb_time_out_saved_flag;    // Store LED flag before timeout so it can be restored when LED is turned on again.

// enum layout_names {
//     _KL=0,       // Keys Layout: The main keyboard layout that has all the characters
//     _FL,         // Function Layout: The function key activated layout with default functions and some added ones
//     _ML,         // Mouse Layout: Mouse Keys and mouse movement
//     _GL,         // GIT Layout: GIT shortcuts and macros
//     _VL,         // VIM Layout: VIM shorcuts and macros
//     _WL,         // Work Layout: Macros for Work.
//     _HL,         // Hack Layout: Macros for pentesting.
//     _GML,        // Game Macro Layout: Macros for games.
//     _EL,         // KDE Layout: Shortcuts for KDE desktop using default KDE shortcuts settings
// };

#define _BL   0     // Base Layer
#define _MSL  2
#define _NL   6     // Numpad Layer
#define _ML   8     // Mod Layer
#define _DLR  12    // HyperCom Layer
#define _DLSR 13    // Hypercom Shift Layer
#define _GM   1     // Game Mode
#define _URL  9     // URL Macro Layer
#define _WL   10    // Work Macro Layer
#define _HL   11    // Hack Layer
#define _GL   7     // Git Macro Layer


enum tapdance_keycodes {
    TD_LGUI_ML = 0,     // Tap dance key to switch to mouse layer _ML
    TD_APP_YL,
    TD_LALT_WL,
    TD_GRV_HL,
    TD_CTRL_TERM,       // Tap dance key to open terminal on LCTRL double press
};

enum ctrl_keycodes {
  L_BRI = SAFE_RANGE,  // LED Brightness Increase
  L_BRD,               // LED Brightness Decrease
  L_PTN,               // LED Pattern Select Next
  L_PTP,               // LED Pattern Select Previous
  L_PSI,               // LED Pattern Speed Increase
  L_PSD,               // LED Pattern Speed Decrease
  L_T_MD,              // LED Toggle Mode
  L_T_ONF,             // LED Toggle On / Off
  L_ON,                // LED On
  L_OFF,               // LED Off
  L_T_BR,              // LED Toggle Breath Effect
  L_T_PTD,             // LED Toggle Scrolling Pattern Direction
  U_T_AUTO,            // USB Extra Port Toggle Auto Detect / Always Active
  U_T_AGCR,            // USB Toggle Automatic GCR control
  DBG_TOG,             // DEBUG Toggle On / Off
  DBG_MTRX,            // DEBUG Toggle Matrix Prints
  DBG_KBD,             // DEBUG Toggle Keyboard Prints
  DBG_MOU,             // DEBUG Toggle Mouse Prints
  MD_BOOT,             // Restart into bootloader after hold timeout
  CYCLE_COLORS,        // Cycles through color instructions from led_instructions.c,
  ROUT_TG,
  ROUT_VI,
  ROUT_VD,
  ROUT_FM,
  M_GIT,               // Spyder team git server macro.
  M_CHIEF,
  M_DEFINE,
  M_MACRO,
  M_J_P,
  M_J_D,
  M_J_S,
  M_J_F,
  M_GRUN,
  M_SPACE,
  M_PSERV,
  M_NMAP,
  SEL_CPY,
  COPY_ALL,
  M_NMAPD,
  TERMINAL
};

enum string_macro_keycodes {
  // The start of this enum should always be equal to end of ctrl_keycodes + 1
  G_INIT = TERMINAL + 1,  // git init
  G_CLONE,                // git clone
  G_CONF,                 // git config --global
  G_ADD,                  // git add
  G_DIFF,                 // git diff
  G_RESET,                // git reset
  G_REBAS,                // git rebase
  G_BRANH,                // git branch
  G_CHECK,                // git checkout
  G_MERGE,                // git merge
  G_REMTE,                // git remote add
  G_FETCH,                // git fetch
  G_PULL,                 // git pull
  G_PUSH,                 // git push
  G_COMM,                 // git commit
  G_STAT,                 // git status
  G_LOG,                  // git log
  G_GUPV,                 // git pull, rebase
};

enum work_string_macro_keycodes {
  W_ROLL = G_LOG + 1,
  W_NC_LO,
  W_ANSIBL,
  W_PLAN,
  W_APPLY,
};

enum hack_string_macro_keycodes {
  H_NMAP = W_APPLY + 1,
  H_PORTS,
  H_IXIO,
  H_HTTP
};

enum url_string_macro_keycodes {
    U_VPPR = H_HTTP + 1,
    U_VPVI,
    U_LV1,
    U_GH,
    U_DDG,
    U_ADL,
};
