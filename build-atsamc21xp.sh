#!/bin/bash
west build -p always -b samc21n_xpro/samc21n18a -- -DDTC_OVERLAY_FILE=boards/atsamc21xp.overlay

