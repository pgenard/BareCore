#include "uart_imx8.h"
#include "imx8mp_clock.h"
#include "imx8mp_clock_parent.h"

static const imx8mp_pll_desc_t *pll_list = imx8mp_pll_descriptions;

static unsigned long frequency_mhz(unsigned long hz) {
    return hz / 1000000UL;
}

/* UART1 parents */

static const imx8mp_clock_parent_t uart1_parents[8] = {
    CLK_PARENT_OSC_24M,
    CLK_PARENT_SYS_PLL1_80M,
    CLK_PARENT_SYS_PLL2_200M,
    CLK_PARENT_SYS_PLL2_100M,
    CLK_PARENT_SYS_PLL3_OUT,
    CLK_PARENT_COUNT,
    CLK_PARENT_COUNT,
    CLK_PARENT_AUDIO_PLL2_OUT
};

/* GPT1 parents */

static const imx8mp_clock_parent_t gpt1_parents[8] = {
    CLK_PARENT_OSC_24M,
    CLK_PARENT_SYS_PLL2_100M,
    CLK_PARENT_SYS_PLL1_400M,
    CLK_PARENT_SYS_PLL1_40M,
    CLK_PARENT_VIDEO_PLL1_OUT,
    CLK_PARENT_SYS_PLL1_80M,
    CLK_PARENT_AUDIO_PLL1_OUT,
    CLK_PARENT_COUNT
};


static void build_parent_frequencies(
    const imx8mp_clock_parent_t *source,
    unsigned long *destination)
{
    unsigned int i;

    for (i = 0; i < 8; i++) {
        if (source[i] == CLK_PARENT_COUNT) {
            destination[i] = 0;
        }
	else {
            destination[i] =
                imx8mp_clock_parent_get(source[i]);
        }
    }
}


static void clock_dump_root(
    const char *name,
    unsigned long offset,
    const imx8mp_clock_parent_t *parent_table)
{
    unsigned long parents[8];

    imx8mp_root_info_t root;

    build_parent_frequencies(
        parent_table,
        parents
    );

    if (imx8mp_clock_root_get(
            offset,
            parents,
            8,
            &root) != 0) {

        uart_write(name);
        uart_write(" unavailable\n");

        return;
    }

    uart_write(name);
    uart_write("     ");

    uart_print_dec(root.mux);
    uart_write("     ");

    uart_print_dec(root.pre_div);
    uart_write("     ");

    uart_print_dec(root.post_div);
    uart_write("     ");

    uart_print_dec(
        frequency_mhz(root.frequency)
    );

    uart_write(" MHz     ");

    uart_write(
        root.enabled ? "ENABLED" : "DISABLED"
    );

    uart_write("\nROOT REG = ");

    uart_print_hex(root.raw_register);

    uart_write("\n");
}


/* PLL diagnostics */

static void clock_dump_plls(void) {
    unsigned int i;
    imx8mp_pll_info_t info;

    uart_write("[PLL Configuration]\n");
    
    uart_write(
        "--------------------------------------------------------------------------\n"
    );

    uart_write(
        "PLL          REF       CONFIG     EFFECTIVE   LOCK   BYPASS   ENABLED\n"
    );
    
    uart_write(
        "--------------------------------------------------------------------------\n"
    );

    for (i = 0; i < imx8mp_pll_description_count; i++) {
        const imx8mp_pll_desc_t *desc =
            &pll_list[i];

        uart_write(imx8mp_pll_descriptions[i].name);
        uart_write("     ");

        if (imx8mp_pll_get_info(
                imx8mp_pll_descriptions[i].base,
                imx8mp_pll_descriptions[i].type,
                &info) != 0) {

            uart_write("UNAVAILABLE\n");

            continue;
        }

        /* Reference */

        if (info.reference_valid) {
            uart_print_dec(
                frequency_mhz(info.reference_frequency)
            );
            uart_write("M");
        }
	else {
            uart_write("UNKNOWN");
        }
        uart_write("     ");


        /* Configured frequency */

        if (info.configured_valid) {
            uart_print_dec(
                frequency_mhz(info.configured_frequency)
            );
            uart_write(" MHz");
        }
	else {
            uart_write("INVALID");
        }

        uart_write("     ");

        /* Effective frequency */

        if (info.valid) {
            uart_print_dec(
                frequency_mhz(info.effective_frequency)
            );
            uart_write(" MHz");
        }
	else {

            uart_write("UNKNOWN");
        }

        uart_write("     ");


        uart_write(
            info.locked ? "YES" : "NO"
        );

        uart_write("      ");


        uart_write(
            info.bypassed ? "YES" : "NO"
        );

        uart_write("       ");


        uart_write(
            info.enabled ? "YES" : "NO"
        );

        uart_write("\n");

        /* Detailed registers 

        uart_write("GEN_CTRL = ");

        uart_print_hex(info.gen_ctrl);

        uart_write(" FDIV0 = ");

        uart_print_hex(info.fdiv_ctl0);


        if (desc->has_fractional_divider) {

            uart_write(" FDIV1 = ");

            uart_print_hex(info.fdiv_ctl1);

        } else {

            uart_write(" LOCK_CTRL = ");

            uart_print_hex(info.lock_ctrl);
	    } */

        /* Bypass diagnostics 

        uart_write(" INTERNAL_BYPASS = ");

        uart_write(
            info.internal_bypass ? "YES" : "NO"
        );

        uart_write(" OUTPUT_BYPASS = ");

        uart_write(
            info.output_bypass ? "YES" : "NO"
        );

        uart_write("\n");
	} */
    }
    
    uart_write(
        "--------------------------------------------------------------------------\n"
    );
}


/* Main diagnostic command */

void imx8mp_clock_dump(void)
{
    uart_write("[i.MX8MP Clock Tree Description]\n");
    uart_write("================================\n");

    clock_dump_plls();

    uart_write("[CCM Clock Roots]\n");
    uart_write(
        "------------------------------------------------\n"
    );

    uart_write(
        "ROOT       MUX   PRE   POST   FREQUENCY   STATE\n"
    );

    uart_write(
        "------------------------------------------------\n"
    );


    clock_dump_root(
        "UART1",
        CCM_UART1_ROOT,
        uart1_parents
    );


    clock_dump_root(
        "GPT1",
        CCM_GPT1_ROOT,
        gpt1_parents
    );


    uart_write(
        "------------------------------------------------\n"
    );
}
