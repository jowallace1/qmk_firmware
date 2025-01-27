# MCU name
MCU = atmega32u4

# Bootloader selection
BOOTLOADER = caterina

# Enable bluetooth
BLUETOOTH_ENABLE = yes
BLUETOOTH_DRIVER = BluefruitLE

# Build Options
#   change yes to no to disable
#
CUSTOM_MATRIX = lite
BOOTMAGIC_ENABLE = no       # Enable Bootmagic Lite
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = yes        # Console for debug
COMMAND_ENABLE = yes        # Commands for debug and configuration
NKRO_ENABLE = yes           # Enable N-Key Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
AUDIO_ENABLE = no           # Audio output

LTO_ENABLE = yes            # Link-time opt

F_CPU = 8000000

SRC += matrix.c