#!/bin/bash
west build -p always -b stm32f3_disco@B/stm32f303xc -- -DDTC_OVERLAY_FILE=boards/stm32f3disco.overlay
