#!/bin/bash
west build -p always -b stm32f3_disco@B/stm32f303xc -- -DEXTRA_DTC_OVERLAY_FILE=app.overlay