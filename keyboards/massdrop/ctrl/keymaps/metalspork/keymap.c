#include "keymap.h"
#include "print.h"

static uint16_t idle_timer;             // Idle LED timeout timer
static uint8_t idle_second_counter;     // Idle LED seconds counter, counts seconds not milliseconds
static uint8_t key_event_counter;       // This counter is used to check if any keys are being held

static const char * sendstring_git_commands[] = {
    "git init ",
    "git clone ",
    "git config --global ",
    "git add ", "git diff ",
    "git reset ",
    "git rebase ",
    "git branch -b \"",
    "git checkout ",
    "git merge ",
    "git remote add ",
    "git fetch ",
    "git pull ",
    "git push ",
    "git commit ",
    "git status ",
    "git log ",
    "git pull --rebase -v",
};

static const char * sendstring_work_commands[] = {
    "cd ~/velo/integration-pipeline/docker-compose\n ./aws-asg-rollout-message-helper", // ROLLOUT
    " | nc localhost 2000", // NC_LOCAL
    "ansible-playbook -i dev_envs.hosts -i tools.hosts -i production_envs.hosts --limit '", // ANSIBLE
    "aws -- ./plan -k ~/velo/keys/deployer -e $ENVIRONMENT", // PLAN
    "aws -- ./plan -k ~/velo/keys/deployer -e $ENVIRONMENT -- apply", // APPLY
};

static const char * sendstring_hack_commands[] = {
    "nmap -sC -sV -p$ports -oA nmap $HOST", // H_NMAP
    "ports=$(nmap -p- --min-rate=1000 -T4 $HOST | grep ^[0-9] | cut -d '/' -f 1 | tr '\n' ',' | sed s/,$//)",
    " | curl -F 'f:1=<-' ix.io", // H_IXIO
    "python -m SimpleHTTPServer\n", // H_HTTP
};

static const char * sendstring_url_commands[] = {
    "velopayments.com",
    "velointra.net",
    "https://forum.level1techs.com/\n",
    "https://forum.admindev.tech/\n",
    "https://github.com/",
    "https://duckduckgo.com/\n"
};

// static const char * sendstring_game_commands[] = {
//     "asdf", //
// };

//Associate our tap dance key with its functionality
qk_tap_dance_action_t tap_dance_actions[] = {
    [TD_LGUI_ML] = ACTION_TAP_DANCE_LAYER_TOGGLE(KC_LGUI, _ML),
    [TD_LALT_WL] = ACTION_TAP_DANCE_LAYER_TOGGLE(KC_LALT, _WL),
    [TD_GRV_HL] = ACTION_TAP_DANCE_LAYER_TOGGLE(KC_GRV, _HL),
    // [TD_APP_YL] = ACTION_TAP_DANCE_LAYER_TOGGLE(KC_APP, _YL),
    [TD_CTRL_TERM] = ACTION_TAP_DANCE_DOUBLE(KC_LCTRL, LCA(KC_T)),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
    [DEFAULT] = LAYOUT(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,           KC_PSCR, KC_SLCK, KC_PAUS,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_INS,  KC_HOME, KC_PGUP,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,  KC_END,  KC_PGDN,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,                            KC_UP,
        KC_LCTL, KC_LGUI, KC_LALT,                   KC_SPC,                             KC_RALT, MO(1),   KC_APP,  KC_RCTL,          KC_LEFT, KC_DOWN, KC_RGHT),
    */
    [_BL] = LAYOUT(
        KC_LGUI,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,             KC_PSCR, KC_SLCK, KC_PAUS, \
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,   KC_INS,  KC_HOME, KC_PGUP, \
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,   KC_DEL,  KC_END,  KC_PGDN, \
        LGUI_T(KC_ESC), KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT, \
        KC_LSPO, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSPC,                              KC_UP, \
        KC_LCTL, MO(_DLR), KC_LALT,                  KC_SPC,                      KC_DEL, KC_TILD, MO(_ML),   KC_RCTL,            KC_LEFT, KC_DOWN, KC_RGHT ),
    [_ML] = LAYOUT(
        _______, DM_PLY1, DM_PLY2, _______, _______, DM_REC1, DM_REC2, _______, _______, DM_RSTP, TG(_GM), KC_WAKE, KC_SLEP,            KC_MUTE, TERM_ON, TERM_OFF, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, ROUT_FM, ROUT_TG, ROUT_VD, ROUT_VI, _______,   KC_MPLY, KC_MSTP, KC_VOLU, \
        L_T_BR,  RGB_SPD, RGB_VAI, RGB_SPI, RGB_HUI, RGB_SAI, _______, U_T_AUTO,U_T_AGCR,_______, _______, _______, _______, _______,   KC_MPRV, KC_MNXT, KC_VOLD, \
        KC_CAPS, RGB_RMOD,RGB_VAD, RGB_MOD, RGB_HUD, RGB_SAD, _______, _______, _______, _______, _______, _______, _______, \
        _______, L_T_MD,  L_T_ONF, _______, _______, MD_BOOT, NK_TOGG, TG(_MSL), _______, _______, _______, _______,                              KC_BRIU, \
        TG(_DLR), L_T_PTD, _______,                  _______,                            _______, TG(_NL), TG(_ML), _______,            _______, KC_BRID, _______ ),
    [_MSL] = LAYOUT(
        _______, KC_ACL0, KC_ACL1, KC_ACL2, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
        _______, KC_BTN4, KC_BTN3, KC_BTN5, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, KC_BTN1, KC_MS_U, KC_BTN2, KC_WH_U, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, KC_MS_L, KC_MS_D, KC_MS_R, KC_WH_D, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, SEL_CPY, _______, _______, _______, _______, _______, _______, _______, _______,                            KC_MS_U,
        _______, TG(_MSL),_______,                   _______,                            _______, _______, TG(_MSL),_______,          KC_MS_L, KC_MS_D, KC_MS_R),
    [_NL] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______,    KC_7,    KC_8,    KC_9, _______, _______, _______, _______,   _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______,    KC_4,    KC_5,    KC_6, _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______,    KC_1,    KC_2,    KC_3, KC_COMM, _______,                              _______, \
        _______, _______, _______,                   _______,                               KC_0,  KC_DOT, _______, _______,            _______, _______, _______ ),
    [_DLR] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, \
        _______, _______, OSL(_WL),_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, \
        _______,OSL(_HL),OSL(_URL),OSL(_GL),_______, _______, KC_LEFT, KC_DOWN, KC_UP, KC_RIGHT, _______, _______, _______, \
        _______, _______,_______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                              _______, \
        _______, _______, _______,                   _______,                            _______, _______, _______, _______,            _______, _______, _______ ),
    [_GM] = LAYOUT(
        KC_LGUI, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, \
        KC_ESC,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
        KC_LSFT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_RSFT,                              _______, \
        _______, _______, _______,                  _______,                             _______, _______, _______, _______,            _______, _______, _______ ),
    [_WL] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, \
        _______, _______, _______, _______, W_ROLL , W_NC_LO, _______, _______, _______,  W_PLAN, W_APPLY, _______, _______, _______,   _______, _______, _______, \
        _______,W_ANSIBL, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                              _______, \
        _______, TG(_WL), _______,                  _______,                             _______, _______, TG(_WL), _______,            _______, _______, _______ ),
    [_HL] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______,  H_IXIO, _______, H_PORTS, _______, _______, _______,   _______, _______, _______, \
        _______, _______, _______, _______, _______, _______,  H_NMAP, _______, _______, _______, _______, _______, _______, \
        _______, _______, _______, _______, _______, _______,  H_HTTP, _______, _______, _______, _______, _______,                              _______, \
        _______, TG(_HL), _______,                  _______,                             _______, _______, TG(_HL), _______,            _______, _______, _______ ),
    [_URL] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______,  U_VPVI, _______,  U_VPPR, _______, _______, _______,   _______, _______, _______, \
        _______,   U_ADL, _______,   U_DDG, _______,    U_GH, _______, _______, _______,   U_LV1, _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                              _______, \
        _______, TG(_URL),_______,                  _______,                             _______, _______, TG(_URL),_______,            _______, _______, _______ ),
    [_GL] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                              _______, \
        _______, _______, _______,                  _______,                             _______, _______, _______, _______,            _______, _______, _______ ),
    /*
    [X] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, TG_NKRO, _______, _______, _______, _______, _______,                            _______,
        _______, _______, _______,                   _______,                            _______, _______, _______, _______,          _______, _______, _______),
    [RGB] = LAYOUT(
        ESC: 0,   F1: 1,    F2: 2,    F3: 3,    F4: 4,    F5: 5,    F6: 6,    F7: 7,    F8: 8,    F9: 9,    F10: 10,  F11: 11,  F12: 12,            PSCR: 13, SLCK: 14, PAUS: 15,
        GRV: 16,  1: 17,    2: 18,    3: 19,    4: 20,    5: 21,    6: 22,    7: 23,    8: 24,    9: 25,    0: 26,    MINS: 27, EQL: 28,  BSPC: 29, INS: 30,  HOME: 31, PGUP: 32,
        TAB: 33,  Q: 34,    W: 35,    E: 36,    R: 37,    T: 38,    Y: 39,    U: 40,    I: 41,    O: 42,    P: 43,    LBRC: 44, RBRC: 45, BSLS: 46, DEL: 47,  END: 48,  PGDN: 49,
        CAPS: 50, A: 51,    S: 52,    D: 53,    F: 54,    G: 55,    H: 56,    J: 57,    K: 58,    L: 59,    SCLN: 60, QUOT: 61, ENT: 62,
        LSFT: 63, Z: 64,    X: 65,    C: 66,    V: 67,    B: 68,    N: 69,    M: 70,    COMM: 71, DOT: 72,  SLSH: 73, RSFT: 74,                               UP: 75,
        LCTL: 76, LGUI: 77, LALT: 78,                   SPC: 79,                                  RALT: 80, Fn: 81,   APP: 82,  RCTL: 83,           LEFT: 84, DOWN: 85, RGHT: 86),
	[MATRIX] = LAYOUT(
	    0,       1,       2,       3,       4,       5,       6,       7,       8,       9,       10,      11,      12,                 13,      14,      15,
	    16,      17,      18,      19,      20,      21,      22,      23,      24,      25,      26,      27,      28,      29,        30,      31,      32,
	    33,      34,      35,      36,      37,      38,      39,      40,      41,      42,      43,      44,      45,      46,        47,      48,      49,
	    50,      51,      52,      53,      54,      55,      56,      57,      58,      59,      60,      61,      62,
	    63,      64,      65,      66,      67,      68,      69,      70,      71,      72,      73,      74,                                   75,
	    76,      77,      78,                        79,                                 80,      81,      82,      83,                 84,      85,      86),
    */
};

#ifdef _______
#undef _______
#define _______ {0, 0, 0}

const uint8_t PROGMEM ledmap[][DRIVER_LED_TOTAL][3] = {
    [_ML] = {
        _______, CORAL,   CORAL,   _______, _______, CORAL,   CORAL,   _______, _______, CORAL,   GREEN  , YELLOW,  YELLOW,           TEAL,    GOLD,   GOLD,
        _______, _______, PINK,    PINK,    PINK,    PINK,    _______, _______, _______, GREEN,   GREEN  , GREEN  ,  GREEN , _______, TEAL,    TEAL,   TEAL,
        ORANGE,  ORANGE,  ORANGE,  ORANGE,  ORANGE,  ORANGE,  _______, AZURE,   AZURE,   _______, _______, _______, _______, _______, TEAL,    TEAL,   TEAL,
        _______, ORANGE,  ORANGE,  ORANGE,  ORANGE,  ORANGE,  _______, _______, _______, _______, _______, _______, _______,
        _______, ORANGE,  _______, CORAL,   _______, AZURE,   AZURE,   BLUE   , _______, _______, _______,   AZURE,                            SPRING,
        _______, _______, _______,                   _______,                            _______, _______,    PINK,   AZURE,          _______, SPRING, _______},
    [_NL] = {
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,   GREEN,   GREEN,   GREEN, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,   GREEN,   GREEN,   GREEN, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,   GREEN,   GREEN,   GREEN,   GREEN, _______,                            _______,
        _______, _______, _______,                   _______,                              GREEN,   GREEN, _______, _______,          _______, _______, _______},
    [_MSL] = {
        _______, GOLD,    GOLD,    GOLD,   _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
        _______, MAGENT,  MAGENT,  MAGENT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, MAGENT,  GOLD,    MAGENT, GOLD,    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, GOLD,    GOLD,    GOLD,   GOLD,    _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, CORAL,  _______, _______, _______, _______, _______, _______, _______, _______,                            GOLD,
        _______, PINK,    _______,                  _______,                                     _______, _______,    PINK, _______, GOLD,    GOLD,    GOLD  },
    [_DLR] = {
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,S_PURPLE, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______,S_PURPLE,S_PURPLE,S_PURPLE, _______, _______,   GREEN,   GREEN,   GREEN,   GREEN, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                            _______,
        _______,    DARK, _______,                   _______,                            _______, _______,    DARK, _______,          _______, _______, _______},

    [_GL] = {
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, BLUE,    AZURE,   AZURE,   _______, CYAN,    _______, TURQ,    PURPLE,  _______, _______, _______, _______, _______,
        _______, CORAL,   GREEN,   GREEN,   TURQ,    _______, _______, _______, _______, GREEN,   _______, _______, _______,
        _______, CYAN,    CHART,   TURQ,    ORANGE,  CHART,   _______, CHART,   _______, _______, _______, _______,                            _______,
        _______, _______, _______,                   _______,                            _______, _______,    PINK, _______,          _______, _______, _______},
    [_WL] = {
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______,  YELLOW,  YELLOW, _______, _______, _______,  PURPLE,  PURPLE, _______, _______, _______, _______, _______, _______,
        _______,    CYAN, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                            _______,
        _______,    DARK, _______,                   _______,                            _______, _______,    DARK, _______,          _______, _______, _______},
    [_HL] = {
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______,   GREEN, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,  GOLDEN, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,    GOLD, _______, _______, _______, _______, _______,                            _______,
        _______, _______, _______,                   _______,                            _______, _______, _______, _______,          _______, _______, _______},
    [_GL] = {
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,  GOLDEN, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,    GOLD, _______, _______, _______, _______, _______,                            _______,
        _______, _______, _______,                   _______,                            _______, _______, _______, _______,          _______, _______, _______},
    [_GM] = {
            RED, _______, _______, _______, _______, _______, _______, _______, _______, _______,    DARK, _______, _______,          _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
            RED, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,     RED,                            _______,
        _______, _______, _______,                   _______,                            _______, _______, _______, _______,          _______, _______, _______},
    [_URL] = {
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______,  GOLDEN, _______,  GOLDEN, _______, _______, _______, _______, _______, _______,
        _______,  ORANGE, _______, YELLOW , _______,    BLUE, _______, _______, _______,    BLUE, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                            _______,
        _______,    DARK, _______,                   _______,                            _______, _______,    DARK, _______,          _______, _______, _______},
};

#undef _______
#define _______ KC_TRNS
#endif

// Runs just one time when the keyboard initializes.
void matrix_init_user(void) {
    // Enable or disable debugging
    debug_enable=true;
    debug_matrix=true;
    debug_keyboard=true;
    debug_mouse=true;
    // gcr_desired=LED_GCR_MAX;
    idle_second_counter = 0;                            // Counter for number of seconds keyboard has been idle.
    key_event_counter = 0;                              // Counter to determine if keys are being held, neutral at 0.
    rgb_time_out_seconds = RGB_DEFAULT_TIME_OUT;        // RGB timeout initialized to its default configure in keymap.h
    rgb_time_out_enable = true;                        // Disable RGB timeout by default. Enable using toggle key.
    rgb_time_out_user_value = false;                    // Has to have the same initial value as rgb_time_out_enable.
    rgb_enabled_flag = true;                            // Initially, keyboard RGB is enabled. Change to false config.h initializes RGB disabled.
    rgb_time_out_fast_mode_enabled = false;             // RGB timeout fast mode disabled initially.
    rgb_time_out_saved_flag = rgb_matrix_get_flags();   // Save RGB matrix state for when keyboard comes back from ide.
};

void keyboard_post_init_user(void) {
    rgb_matrix_enable();
    // gcr_desired=75;
}

// Runs constantly in the background, in a loop.
void matrix_scan_user(void) {
    if(rgb_time_out_enable && rgb_enabled_flag) {
        // If the key event counter is not zero then some key was pressed down but not released, thus reset the timeout counter.
        if (key_event_counter) {
            idle_second_counter = 0;
        } else if (timer_elapsed(idle_timer) > MILLISECONDS_IN_SECOND) {
            idle_second_counter++;
            idle_timer = timer_read();
        }

        if (idle_second_counter >= rgb_time_out_seconds) {
            rgb_time_out_saved_flag = rgb_matrix_get_flags();
            rgb_matrix_set_flags(LED_FLAG_NONE);
            rgb_matrix_disable_noeeprom();
            rgb_enabled_flag = false;
            idle_second_counter = 0;
        }
    }
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint32_t key_timer;

    // Increment key event counter for every press and decrement for every release.
    if (record->event.pressed) {
        key_event_counter++;
    } else {
        key_event_counter--;
    }

    if (rgb_time_out_enable) {
        idle_timer = timer_read();
        // Reset the seconds counter. Without this, something like press> leave x seconds> press, would be x seconds on the effective counter not 0 as it should.
        idle_second_counter = 0;
        if (!rgb_enabled_flag) {
            rgb_matrix_enable_noeeprom();
            rgb_matrix_set_flags(rgb_time_out_saved_flag);
            rgb_enabled_flag = true;
        }
    }

    switch (keycode) {
        case L_BRI:
            if (record->event.pressed) {
                if (LED_GCR_STEP > LED_GCR_MAX - gcr_desired) gcr_desired = LED_GCR_MAX;
                else gcr_desired += LED_GCR_STEP;
            }
            return false;
        case L_BRD:
            if (record->event.pressed) {
                if (LED_GCR_STEP > gcr_desired) gcr_desired = 0;
                else gcr_desired -= LED_GCR_STEP;
            }
            return false;
        case U_T_AUTO:
            if (record->event.pressed && MODS_SHIFT && MODS_CTRL) {
                TOGGLE_FLAG_AND_PRINT(usb_extra_manual, "USB extra port manual mode");
            }
            return false;
        case U_T_AGCR:
            if (record->event.pressed && MODS_SHIFT && MODS_CTRL) {
                TOGGLE_FLAG_AND_PRINT(usb_gcr_auto, "USB GCR auto mode");
            }
            return false;
        case MD_BOOT:
            if (record->event.pressed) {
                key_timer = timer_read32();
            } else {
                if (timer_elapsed32(key_timer) >= 500) {
                    reset_keyboard();
                }
            }
            return false;
    }

    if (record->event.pressed) {
        // The Yakuake config uses LGUI+Ctrl+Shift+<keycode>. KC_NO used for undesired key, all mapped ones get mods.
        // if (get_highest_layer(layer_state) == _YL && keycode != TG(_YL)) {
        //     tap_code16(C(G(S(keycode))));
        //     return false;
        // }
        switch (keycode) {
            case DBG_TOG:
                TOGGLE_FLAG_AND_PRINT(debug_enable, "Debug mode");
                return false;
            case DBG_MTRX:
                TOGGLE_FLAG_AND_PRINT(debug_matrix, "Debug matrix");
                return false;
            case DBG_KBD:
                TOGGLE_FLAG_AND_PRINT(debug_keyboard, "Debug keyboard");
                return false;
            case DBG_MOU:
                TOGGLE_FLAG_AND_PRINT(debug_mouse, "Debug mouse");
                return false;
            case RGB_TOG:
                rgb_time_out_enable = rgb_time_out_user_value;
                switch (rgb_matrix_get_flags()) {
                    case LED_FLAG_ALL: {
                        rgb_matrix_set_flags(LED_FLAG_KEYLIGHT | LED_FLAG_MODIFIER);
                        rgb_matrix_set_color_all(0, 0, 0);
                    }
                    break;
                    case LED_FLAG_KEYLIGHT | LED_FLAG_MODIFIER: {
                        rgb_matrix_set_flags(LED_FLAG_UNDERGLOW);
                        rgb_matrix_set_color_all(0, 0, 0);
                    }
                    break;
                    case LED_FLAG_UNDERGLOW: {
                        // This line is for LED idle timer. It disables the toggle so you can turn off LED completely if you like
                        rgb_time_out_enable = false;
                        rgb_matrix_set_flags(LED_FLAG_NONE);
                        rgb_matrix_disable_noeeprom();
                    }
                    break;
                    default: {
                        rgb_matrix_set_flags(LED_FLAG_ALL);
                        rgb_matrix_enable_noeeprom();
                    }
                    break;
                }
                return false;
            // ======================================================== CUSTOM KEYCODES BELOW ========================================================
            case ROUT_TG:
                // Toggle idle LED timeout on or off
                rgb_time_out_enable = !rgb_time_out_enable;
                rgb_time_out_user_value = rgb_time_out_enable;
                return false;
            case ROUT_VI:
                // Increase idle LED timeout value in seconds
                // Only increase if current value is lower than RGB_TIME_OUT_MAX. Don't care what value the result will be
                // Modity RGB_TIME_OUT_STEP for bigger or smaller increments
                if (!rgb_time_out_fast_mode_enabled && rgb_time_out_seconds <= RGB_TIME_OUT_MAX) {
                    rgb_time_out_seconds += RGB_TIME_OUT_STEP;
                }
                return false;
            case ROUT_VD:
                // Decrease idle LED timeout value in seconds
                // Only decrease if current value is higher than minimum value and the result is larger than zero
                // Modity RGB_TIME_OUT_STEP for bigger or smaller decrements
                if (!rgb_time_out_fast_mode_enabled && rgb_time_out_seconds > RGB_TIME_OUT_MIN) {
                    rgb_time_out_seconds -= RGB_TIME_OUT_STEP;
                }
                return false;
            case COPY_ALL:
                // Selects all and text and copy
                SEND_STRING(SS_LCTRL("ac"));
                return false;
            case SEL_CPY:
                // Select word under cursor and copy. Double mouse click then ctrl+c
                tap_code16(KC_BTN1);
                tap_code16(KC_BTN1);
                tap_code16(C(KC_C));
                return false;
            case ROUT_FM:
                if (rgb_time_out_fast_mode_enabled) {
                    rgb_time_out_seconds = rgb_time_out_saved_seconds;
                } else {
                    rgb_time_out_saved_seconds = rgb_time_out_seconds;
                    rgb_time_out_seconds = RGB_FAST_MODE_TIME_OUT;
                }
                rgb_time_out_fast_mode_enabled = !rgb_time_out_fast_mode_enabled;
                return false;
            case G_INIT ... G_LOG:
                send_string_with_delay(sendstring_git_commands[keycode - G_INIT], 5);
                return false;
            case W_ROLL ... W_APPLY:
                send_string_with_delay(sendstring_work_commands[keycode - W_ROLL], 5);
                return false;
            case H_NMAP ... H_HTTP:
                send_string_with_delay(sendstring_hack_commands[keycode - H_NMAP], 5);
                return false;
            case U_VPPR ... U_ADL:
                send_string_with_delay(sendstring_url_commands[keycode - U_VPPR], 5);
                return false;
        }
    }
    return true;
}

void set_layer_color(int layer) {
    if (layer == 0) { return; }
    for (int i = 0; i < DRIVER_LED_TOTAL; i++) {
        HSV hsv = {
            .h = pgm_read_byte(&ledmap[layer][i][0]),
            .s = pgm_read_byte(&ledmap[layer][i][1]),
            .v = pgm_read_byte(&ledmap[layer][i][2]),
        };
        if (hsv.h || hsv.s || hsv.v) {
            RGB rgb = hsv_to_rgb(hsv);
            float f = (float)rgb_matrix_config.hsv.v / UINT8_MAX;
            rgb_matrix_set_color(i, f * rgb.r, f * rgb.g, f * rgb.b);
        } else if (layer != 6) {
            // Only deactivate non-defined key LEDs at layers other than FN. Because at FN we have RGB adjustments and need to see them live.
            // If the values are all false then it's a transparent key and deactivate LED at this layer
            // rgb_matrix_set_color(i, 0, 0, 0);
        }
    }
}

void rgb_matrix_indicators_user(void) {
    if (g_suspend_state || disable_layer_color ||
        rgb_matrix_get_flags() == LED_FLAG_NONE ||
        rgb_matrix_get_flags() == LED_FLAG_UNDERGLOW) {
            return;
        }
    set_layer_color(get_highest_layer(layer_state));
}

/* This is a test function for Raw HID, which is currently not implemented for this keyboard */

void raw_hid_receive(uint8_t *data, uint8_t length) {

}

