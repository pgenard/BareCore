#include "imx8mp_clock_pll.h"

#define PLL_1443X_DESC(n, b) {             \
    .name = n,                             \
    .base = b,                             \
    .type = IMX8MP_PLL_1443X,              \
    .gen_ctrl_offset = PLL_GEN_CTRL,       \
    .div0_offset = PLL_FDIV_CTL0,          \
    .div1_offset = PLL_FDIV_CTL1,          \
    .lock_ctrl_offset = 0xFFFFFFFFU,       \
    .bypass_mux_mask = PLL_OUTPUT_BYPASS_MUX, \
    .has_fractional_divider = 1            \
}

#define PLL_1416X_DESC(n, b) {             \
    .name = n,                             \
    .base = b,                             \
    .type = IMX8MP_PLL_1416X,              \
    .gen_ctrl_offset = PLL_GEN_CTRL,       \
    .div0_offset = PLL_FDIV_CTL0,          \
    .div1_offset = 0xFFFFFFFFU,            \
    .lock_ctrl_offset = PLL_LOCK_CTRL,     \
    .bypass_mux_mask = PLL_OUTPUT_BYPASS_MUX, \
    .has_fractional_divider = 0            \
}

const imx8mp_pll_desc_t imx8mp_pll_descriptions[] = {
    PLL_1443X_DESC("AUDIO_PLL1", PLL_AUDIO1_BASE),
    PLL_1443X_DESC("AUDIO_PLL2", PLL_AUDIO2_BASE),
    PLL_1443X_DESC("VIDEO_PLL1", PLL_VIDEO1_BASE),
    PLL_1443X_DESC("DRAM_PLL", PLL_DRAM_BASE),
    PLL_1416X_DESC("GPU_PLL", PLL_GPU_BASE),
    PLL_1416X_DESC("VPU_PLL", PLL_VPU_BASE),
    PLL_1416X_DESC("ARM_PLL", PLL_ARM_BASE),
    PLL_1416X_DESC("SYS_PLL1", PLL_SYS1_BASE),
    PLL_1416X_DESC("SYS_PLL2", PLL_SYS2_BASE),
    PLL_1416X_DESC("SYS_PLL3", PLL_SYS3_BASE)
};

const unsigned int imx8mp_pll_description_count =
    sizeof(imx8mp_pll_descriptions) /
    sizeof(imx8mp_pll_descriptions[0]);

const imx8mp_pll_reference_t imx8mp_pll_default_reference = {
    .osc_24m_hz = IMX8MP_OSC_FREQ
};
