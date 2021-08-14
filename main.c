/******************************************************************************
* File Name:   main.c
*
* Description: In this code example, ADC HAL (Hardware Abstraction Layer) driver
*              is configured to sample input voltage and send it to the UART.
*              This code is based on ADC_basic Application example.
*
* Related Document: See README.md
*
*
*******************************************************************************
* Copyright 2020-2021, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/*******************************************************************************
* Macros
*******************************************************************************/
/*
 * Single channel configuration uses channel 2 in single ended mode.
 */
/* Channel 2 input pin */
#define VPLUS_CHANNEL_2             (P10_2)
#define UNTITLED

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
/* Single channel initialization function*/
void adc_single_channel_init(void);

/* Function to read input voltage from channel 0 */
void adc_single_channel_process(void);

/*******************************************************************************
* Global Variables
*******************************************************************************/
/* ADC Object */
cyhal_adc_t adc_obj;

/* ADC Channel 2 Object */
cyhal_adc_channel_t adc_chan_2_obj;

/* Default ADC configuration */
const cyhal_adc_config_t adc_config = {
        .continuous_scanning=false, // Continuous Scanning is disabled
        .average_count=1,           // Average count disabled
        .vref=CYHAL_ADC_REF_VDDA,   // VREF for Single ended channel set to VDDA
        .vneg=CYHAL_ADC_VNEG_VSSA,  // VNEG for Single ended channel set to VSSA
        .resolution = 12u,          // 12-bit resolution
        .ext_vref = NC,             // No connection
        .bypass_pin = NC };       // No connection

/* Variable for storing character read from terminal */
uint8_t uart_read_value;

/* Run or Stop ADC sampling flag. */
bool adc_run_flag = true;

/* Sampling interval*/
uint32_t sampling_period = 1000;

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function for CM4 CPU. It does...
*    1. Configure and initialize ADC.
*    2. Every 200ms read the input voltage and display input voltage on UART.
*
* Parameters:
*  none
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    /* Variable to capture return value of functions */
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    
    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
                                 CY_RETARGET_IO_BAUDRATE);

    /* retarget-io init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

#ifndef UNTITLED
    /* Print message */
    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");
    printf("-------------------------------------------------------------\r\n");
    printf("PSoC 6 MCU: ADC controlled sampling.\r\n");
    printf("Provide input voltage at P10_2 with different sampling steps.\r\n");
    printf("-------------------------------------------------------------\r\n\n");
#endif

    /* Initialize Channel 0 */
    adc_single_channel_init();

    /* Update ADC configuration */
    result = cyhal_adc_configure(&adc_obj, &adc_config);
    if(result != CY_RSLT_SUCCESS)
    {
        printf("ADC configuration update failed. Error: %ld\n", (long unsigned int)result);
        CY_ASSERT(0);
    }

    for (;;)
    {
        /* Check UART input */
        if (cyhal_uart_getc(&cy_retarget_io_uart_obj, &uart_read_value, 1)
             == CY_RSLT_SUCCESS)
        {
            switch (uart_read_value) {
            case 'r':
            	adc_run_flag = true;
            	break;
            case 'R':
                adc_run_flag = true;
                break;
            case 's':
            	adc_run_flag = false;
            	break;
            case 'S':
                adc_run_flag = false;
                break;
            case '1':
            	sampling_period = 1000;
                break;
            case '2':
            	sampling_period = 2000;
                break;
            case '3':
            	sampling_period = 3000;
                break;
            case '4':
            	sampling_period = 4000;
                break;
            case '5':
            	sampling_period = 5000;
                break;
            case '6':
            	sampling_period = 6000;
                break;
            case '7':
            	sampling_period = 7000;
                break;
            case '8':
            	sampling_period = 8000;
                break;
            case '9':
            	sampling_period = 9000;
                break;
            }
        }

        if (adc_run_flag)
        {
            /* Sample input voltage at channel 0 */
            adc_single_channel_process();

            /* Sampling period - delay between scans */
            cyhal_system_delay_ms(sampling_period);
        }
    }
}

/*******************************************************************************
 * Function Name: adc_single_channel_init
 *******************************************************************************
 *
 * Summary:
 *  ADC single channel initialization function. This function initializes and
 *  configures channel 2 of ADC.
 *
 * Parameters:
 *  void
 *
 * Return:
 *  void
 *
 *******************************************************************************/
void adc_single_channel_init(void)
{
    /* Variable to capture return value of functions */
    cy_rslt_t result;

    /* Initialize ADC. The ADC block which can connect to pin 10[2] is selected */
    result = cyhal_adc_init(&adc_obj, VPLUS_CHANNEL_2, NULL);
    if(result != CY_RSLT_SUCCESS)
    {
        printf("ADC initialization failed. Error: %ld\n", (long unsigned int)result);
        CY_ASSERT(0);
    }

    /* ADC channel configuration */
    const cyhal_adc_channel_config_t channel_config = {
            .enable_averaging = false,  // Disable averaging for channel
            .min_acquisition_ns = 1000, // Minimum acquisition time set to 1us
            .enabled = true };          // Sample this channel when ADC performs a scan

    /* Initialize a channel 2 and configure it to scan P10_2 in single ended mode. */
    result  = cyhal_adc_channel_init_diff(&adc_chan_2_obj, &adc_obj, VPLUS_CHANNEL_2,
                                          CYHAL_ADC_VNEG, &channel_config);
    if(result != CY_RSLT_SUCCESS)
    {
        printf("ADC single ended channel initialization failed. Error: %ld\n", (long unsigned int)result);
        CY_ASSERT(0);
    }

#ifndef UNTITLED
    printf("ADC is configured in single channel configuration\r\n\n");
    printf("Provide input voltage at pin P10_2. \r\n\n");
#endif

}

/*******************************************************************************
 * Function Name: adc_single_channel_process
 *******************************************************************************
 *
 * Summary:
 *  ADC single channel process function. This function reads the input voltage
 *  and prints the input voltage on UART.
 *
 * Parameters:
 *  void
 *
 * Return:
 *  void
 *
 *******************************************************************************/
void adc_single_channel_process(void)
{
    /* Variable to store ADC conversion result from channel 2 */
    int32_t adc_result_2 = 0;

    /* Read input voltage, convert it to millivolts and print input voltage */
    adc_result_2 = cyhal_adc_read_uv(&adc_chan_2_obj)/1000;

#ifndef  UNTITLED
    printf("V = %4ld mV  T = %2d s\r\n", (long int)adc_result_2,(int)sampling_period/1000);
#else
    printf("%4ld\r\n", (long int)adc_result_2);
#endif
}

/* [] END OF FILE */
