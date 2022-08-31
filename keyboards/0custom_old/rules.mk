# MCU name
MCU = atmega32u4

# Bootloader selection
BOOTLOADER = caterina

# Bluetooth options
# BLUETOOTH_ENABLE = yes
# BLUETOOTH_DRIVER = BluefruitLE

# Build Options
#	change yes to no to disable
#
CUSTOM_MATRIX = lite
NKRO_ENABLE = no            # Enable N-Key Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
AUDIO_ENABLE = no           # Audio output
MOUSEKEY_ENABLE = no       # Mouse keys
EXTRAKEY_ENABLE = no       # Audio control and System control

LTO_ENABLE = yes
KEY_LOCK_ENABLE = no

SRC += matrix.c

F_CPU = 8000000

# Debug options

# CONSOLE_ENABLE = yes