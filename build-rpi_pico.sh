#!/bin/bash
west build -p always -b rpi_pico/rp2040 -- -DEXTRA_CONF_FILE=boards/rpi_pico.conf -DDTC_OVERLAY_FILE=boards/rpi_pico.overlay -DDTC_OVERLAY_FILE=boards/rpi_pico.overlay
