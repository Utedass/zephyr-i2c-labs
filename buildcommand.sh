#/bin/bash
west build -p always -b esp32c6_devkitc/esp32c6 -- -DDTC_OVERLAY_FILE=boards/esp32c6.overlay
