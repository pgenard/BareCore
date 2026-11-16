#include "imx8mp_clock.h"
#include "imx8mp_clock_parent.h"

struct clock_factor {
    unsigned long pll_base;
    imx8mp_pll_type_t pll_type;
    unsigned int divisor;
};

static const struct clock_factor parent_table[CLK_PARENT_COUNT] = {

    [CLK_PARENT_OSC_24M] =
        { 0, IMX8MP_PLL_1416X, 1 },

    [CLK_PARENT_SYS_PLL1_OUT] =
        { PLL_SYS1_BASE, IMX8MP_PLL_1416X, 1 },

    [CLK_PARENT_SYS_PLL1_40M] =
        { PLL_SYS1_BASE, IMX8MP_PLL_1416X, 20 },

    [CLK_PARENT_SYS_PLL1_80M] =
        { PLL_SYS1_BASE, IMX8MP_PLL_1416X, 10 },

    [CLK_PARENT_SYS_PLL1_100M] =
        { PLL_SYS1_BASE, IMX8MP_PLL_1416X, 8 },

    [CLK_PARENT_SYS_PLL1_133M] =
        { PLL_SYS1_BASE, IMX8MP_PLL_1416X, 6 },

    [CLK_PARENT_SYS_PLL1_160M] =
        { PLL_SYS1_BASE, IMX8MP_PLL_1416X, 5 },

    [CLK_PARENT_SYS_PLL1_200M] =
        { PLL_SYS1_BASE, IMX8MP_PLL_1416X, 4 },

    [CLK_PARENT_SYS_PLL1_266M] =
        { PLL_SYS1_BASE, IMX8MP_PLL_1416X, 3 },

    [CLK_PARENT_SYS_PLL1_400M] =
        { PLL_SYS1_BASE, IMX8MP_PLL_1416X, 2 },

    [CLK_PARENT_SYS_PLL1_800M] =
        { PLL_SYS1_BASE, IMX8MP_PLL_1416X, 1 },

    [CLK_PARENT_SYS_PLL2_OUT] =
        { PLL_SYS2_BASE, IMX8MP_PLL_1416X, 1 },

    [CLK_PARENT_SYS_PLL2_50M] =
        { PLL_SYS2_BASE, IMX8MP_PLL_1416X, 20 },

    [CLK_PARENT_SYS_PLL2_100M] =
        { PLL_SYS2_BASE, IMX8MP_PLL_1416X, 10 },

    [CLK_PARENT_SYS_PLL2_125M] =
        { PLL_SYS2_BASE, IMX8MP_PLL_1416X, 8 },

    [CLK_PARENT_SYS_PLL2_166M] =
        { PLL_SYS2_BASE, IMX8MP_PLL_1416X, 6 },

    [CLK_PARENT_SYS_PLL2_200M] =
        { PLL_SYS2_BASE, IMX8MP_PLL_1416X, 5 },

    [CLK_PARENT_SYS_PLL2_250M] =
        { PLL_SYS2_BASE, IMX8MP_PLL_1416X, 4 },

    [CLK_PARENT_SYS_PLL2_333M] =
        { PLL_SYS2_BASE, IMX8MP_PLL_1416X, 3 },

    [CLK_PARENT_SYS_PLL2_500M] =
        { PLL_SYS2_BASE, IMX8MP_PLL_1416X, 2 },

    [CLK_PARENT_SYS_PLL2_1000M] =
        { PLL_SYS2_BASE, IMX8MP_PLL_1416X, 1 },

    [CLK_PARENT_SYS_PLL3_OUT] =
        { PLL_SYS3_BASE, IMX8MP_PLL_1416X, 1 },

    [CLK_PARENT_AUDIO_PLL1_OUT] =
        { PLL_AUDIO1_BASE, IMX8MP_PLL_1443X, 1 },

    [CLK_PARENT_AUDIO_PLL2_OUT] =
        { PLL_AUDIO2_BASE, IMX8MP_PLL_1443X, 1 },

    [CLK_PARENT_VIDEO_PLL1_OUT] =
        { PLL_VIDEO1_BASE, IMX8MP_PLL_1443X, 1 },

    [CLK_PARENT_GPU_PLL_OUT] =
        { PLL_GPU_BASE, IMX8MP_PLL_1416X, 1 },

    [CLK_PARENT_VPU_PLL_OUT] =
        { PLL_VPU_BASE, IMX8MP_PLL_1416X, 1 },

    [CLK_PARENT_ARM_PLL_OUT] =
        { PLL_ARM_BASE, IMX8MP_PLL_1416X, 1 }

};

unsigned long imx8mp_clock_parent_get(imx8mp_clock_parent_t parent) {
    const struct clock_factor *factor;
    unsigned long frequency;

    if ((unsigned int)parent >= CLK_PARENT_COUNT)
        return 0;

    if (parent == CLK_PARENT_OSC_24M)
        return IMX8MP_OSC_FREQ;

    factor = &parent_table[parent];

    if (factor->divisor == 0)
        return 0;

    imx8mp_pll_info_t info;

    if (imx8mp_pll_get_info(
        factor->pll_base,
        factor->pll_type,
        &info) != 0)
        return 0;

    if (!info.valid)
        return 0;

    frequency = info.frequency;

    return frequency / factor->divisor;
}
