# RGBLIGHT_ENABLE = no				# Not for MD boards. This is here in case you forget.
COMMAND_ENABLE = no				# Commands for debug and configuration
# BOOTMAGIC_ENABLE = full			# Virtual DIP switch configuration
# BOOTMAGIC_ENABLE = lite
# AUTO_SHIFT_ENABLE = yes      	# Auto Shift
NKRO_ENABLE = yes           	# USB Nkey Rollover
DYNAMIC_MACRO_ENABLE = yes		# Dynamic macro recording and play
MOUSEKEY_ENABLE = yes			# Enable mouse control keycodes. Increases firmware size.
TAP_DANCE_ENABLE = yes			# Enable tap dance keys
CONSOLE_ENABLE = no			# Enable debugging console. Increases firmware size.
SRC += config_led.c				# Used to add files to the compilation/linking list.
# SRC += matrix.c
TERMINAL_ENABLE = no
EXTRAKEY_ENABLE = yes			# Audio control and System control
RAW_ENABLE = no				# Raw HID has not yet been implemented for this keyboard
# COMBO_ENABLE					# Key combo feature
# LEADER_ENABLE					# Enable leader key chording

CUSTOM_MATRIX = yes
RGB_MATRIX_ENABLE = custom


BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality on B7 by default
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
MIDI_ENABLE = no            # MIDI support
UNICODE_ENABLE = no         # Unicode
BLUETOOTH_ENABLE = no       # Enable Bluetooth with the Adafruit EZ-Key HID
AUDIO_ENABLE = no           # Audio output on port C6
FAUXCLICKY_ENABLE = no      # Use buzzer to emulate clicky switches
HD44780_ENABLE = no         # Enable support for HD44780 based LCDs
VIRTSER_ENABLE = no         # USB Serial Driver
RAW_ENABLE = no             # Raw device
AUTO_SHIFT_ENABLE = no      # Auto Shift

DEBOUNCE_TYPE = eager_pk
DEBOUNCE = 52
