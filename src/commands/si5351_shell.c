#include "si5351_shell.h"
#include <zephyr/shell/shell.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/clock_control/si5351.h>
#include <stdio.h>
#include <stdlib.h>

static int cmd_si5351_status(const struct shell *shell, size_t argc, char **argv)
{
    const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(cg1));
    if (!device_is_ready(dev))
    {
        shell_print(shell, "SI5351 device not ready");
        return -ENODEV;
    }

    si5351_status_t status;
    int ret = si5351_get_status(dev, &status);
    if (ret < 0)
    {
        shell_print(shell, "Failed to get SI5351 status: %d", ret);
        return ret;
    }

    shell_print(shell, "SI5351 Status:");
    shell_print(shell, "System Init: %s", status.sys_init ? "Yes" : "No");
    shell_print(shell, "PLL A Loss of Lock: %s", status.plla_loss_of_lock ? "Yes" : "No");
    shell_print(shell, "PLL B Loss of Lock: %s", status.pllb_loss_of_lock ? "Yes" : "No");
    shell_print(shell, "CLKin Loss of Signal: %s", status.clkin_loss_of_signal ? "Yes" : "No");
    shell_print(shell, "Xtal Loss of Signal: %s", status.xtal_loss_of_signal ? "Yes" : "No");
    shell_print(shell, "Revision ID: %d", status.revision_id);

    return 0;
}

static int cmd_si5351_tune_pll(const struct shell *shell, size_t argc, char **argv)
{
    if (argc < 5)
    {
        shell_print(shell, "Usage: si5351 tune_pll <pll_mask> <p1> <p2> <p3>");
        return -EINVAL;
    }

    const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(cg1));
    if (!device_is_ready(dev))
    {
        shell_print(shell, "SI5351 device not ready");
        return -ENODEV;
    }

    uint8_t pll_mask = atoi(argv[1]);
    uint32_t p1 = atoi(argv[2]);
    uint32_t p2 = atoi(argv[3]);
    uint32_t p3 = atoi(argv[4]);

    si5351_pll_parameters_t pll_params = {
        .p1 = p1,
        .p2 = p2,
        .p3 = p3,
    };

    if (pll_mask != si5351_pll_mask_a && pll_mask != si5351_pll_mask_b)
    {
        shell_print(shell, "Invalid PLL mask. Use 1 for PLL A or 2 for PLL B.");
        return -EINVAL;
    }

    int ret = si5351_tune_pll(dev, pll_mask, &pll_params);
    if (ret < 0)
    {
        shell_print(shell, "Failed to tune PLL: %d", ret);
        return ret;
    }

    shell_print(shell, "PLL tuned successfully: mask=%d, p1=%u, p2=%u, p3=%u", pll_mask, p1, p2, p3);
    return 0;
}

static int cmd_si5351_reset_pll(const struct shell *shell, size_t argc, char **argv)
{
    if (argc < 2)
    {
        shell_print(shell, "Usage: si5351 reset_pll <pll_mask>");
        return -EINVAL;
    }

    int pll_mask = atoi(argv[1]);
    const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(cg1));
    if (!device_is_ready(dev))
    {
        shell_print(shell, "SI5351 device not ready");
        return -ENODEV;
    }

    int ret = si5351_reset_pll(dev, pll_mask);
    if (ret < 0)
    {
        shell_print(shell, "Failed to reset PLL: %d", ret);
        return ret;
    }
    shell_print(shell, "PLL reset successfully");
    return 0;
}

static int cmd_si5351_set_output(const struct shell *shell, size_t argc, char **argv)
{
    if (argc < 2)
    {
        shell_print(shell, "Usage: si5351 enable_output <output_id> <state>");
        return -EINVAL;
    }

    int output_id = atoi(argv[1]);
    int state_str = atoi(argv[2]);
    si5351_output_output_t state = (state_str) ? si5351_output_output_enabled : si5351_output_output_disabled;

    const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(cg1));
    if (!device_is_ready(dev))
    {
        shell_print(shell, "SI5351 device not ready");
        return -ENODEV;
    }

    int status = si5351_set_output(dev, output_id, state);
    if (status < 0)
    {
        shell_print(shell, "Failed to set output %d: %d", output_id, status);
        return status;
    }

    shell_print(shell, "Output %d enabled successfully", output_id);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_si5351,
                               SHELL_CMD(status, NULL, "Show SI5351 status", cmd_si5351_status),
                               SHELL_CMD(tune_pll, NULL, "Tune SI5351 PLL parameters", cmd_si5351_tune_pll),
                               SHELL_CMD(reset_pll, NULL, "Soft reset pll", cmd_si5351_reset_pll),
                               SHELL_CMD(set_output, NULL, "Enable or disable output", cmd_si5351_set_output),
                               SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(si5351, &sub_si5351, "SI5351 commands", NULL);
