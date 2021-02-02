/*******************************************************************************
* File Name: cloud_task.c
*
* Description: This file contains the task that handles cloud connectivity.
*
********************************************************************************
* (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
********************************************************************************
* This software, including source code, documentation and related materials
* (“Software”), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries (“Cypress”) and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software (“EULA”).
*
* If no EULA applies, Cypress hereby grants you a personal, nonexclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress’s integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death (“High Risk Product”). By
* including Cypress’s product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*****************************************​**************************************/


/******************************************************************************
* Header files includes
******************************************************************************/
#include "cybsp.h"
#include "cyhal.h"
#include "cycfg.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cy_secure_sockets.h"
#include "cy_wcm.h"
#include <stdlib.h>
#include <stdio.h>

#include "cloud_task.h"
#include "wifi_config.h"

/*******************************************************************************
* Global constants
*******************************************************************************/

/*******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Global variables
******************************************************************************/

/*******************************************************************************
* Function Name: task_cloud
********************************************************************************
* Summary:
*  Task that initializes the connection to the cloud and handles MQTT messages.
*
* Parameters:
*  void *param : Task parameter defined during task creation (unused)
*
*******************************************************************************/
void task_cloud(void* param)
{

    /* Remove warning for unused parameter */
    (void)param;

	cy_rslt_t result;
	cy_wcm_connect_params_t connect_param;
	cy_wcm_ip_address_t ip_address;
	uint32_t retry_count;

    /* Connect to WiFi */
	/* Configure the interface as a Wi-Fi STA (i.e. Client). */
	cy_wcm_config_t config = {.interface = CY_WCM_INTERFACE_TYPE_STA};

	/* Initialize the Wi-Fi Connection Manager and return if the operation fails. */
	result = cy_wcm_init(&config);

	printf("\nWi-Fi Connection Manager initialized.\n");

	/* Configure the connection parameters for the Wi-Fi interface. */
	memset(&connect_param, 0, sizeof(cy_wcm_connect_params_t));
	memcpy(connect_param.ap_credentials.SSID, WIFI_SSID, sizeof(WIFI_SSID));
	memcpy(connect_param.ap_credentials.password, WIFI_PASSWORD, sizeof(WIFI_PASSWORD));
	connect_param.ap_credentials.security = WIFI_SECURITY;

	/* Connect to the Wi-Fi AP. */
	for (retry_count = 0; retry_count < MAX_WIFI_CONN_RETRIES; retry_count++)
	{
		printf("Connecting to Wi-Fi AP '%s'\n", connect_param.ap_credentials.SSID);
		result = cy_wcm_connect_ap(&connect_param, &ip_address);

		if (result == CY_RSLT_SUCCESS)
		{
			printf("Successfully connected to Wi-Fi network '%s'.\n",
					connect_param.ap_credentials.SSID);

			// Print IP Address
			if (ip_address.version == CY_WCM_IP_VER_V4)
			{
				printf("IPv4 Address Assigned: %d.%d.%d.%d\n", (uint8_t)ip_address.ip.v4,
						(uint8_t)(ip_address.ip.v4 >> 8), (uint8_t)(ip_address.ip.v4 >> 16),
						(uint8_t)(ip_address.ip.v4 >> 24));
			}
			else if (ip_address.version == CY_WCM_IP_VER_V6)
			{
				printf("IPv6 Address Assigned: %0X:%0X:%0X:%0X\n", (unsigned int)ip_address.ip.v6[0],
						(unsigned int)ip_address.ip.v6[1], (unsigned int)ip_address.ip.v6[2],
						(unsigned int)ip_address.ip.v6[3]);
			}

			break; /* Exit the for loop once the connection has been made */
		}
	}

	if(result != CY_RSLT_SUCCESS)
	{
		printf("Connect to WiFi Failed!\n");
	}

    /* Repeatedly running part of the task */
    /* Nothing to do here (for now) so we will exit the task */
    for(;;)
    {
    	vTaskDelete(NULL);
    }
}

/* END OF FILE [] */
