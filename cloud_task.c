#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "cy_wcm.h"

#include "cloud_task.h"

#define CLOUD_WIFI_AP        "ew2021"
#define CLOUD_WIFI_PW        "ew2021ap"
#define CLOUD_WIFI_SECURITY  CY_WCM_SECURITY_WPA2_MIXED_PSK
#define CLOUD_WIFI_BAND      CY_WCM_WIFI_BAND_ANY

static void cloud_connectWifi();

void cloud_task(void* param)
{
    (void)param;

	cloud_connectWifi();

	while(1)
	{
		vTaskDelay(1);

	}
}

static void cloud_connectWifi()
{
	cy_rslt_t result;

	cy_wcm_connect_params_t connect_param = {
		.ap_credentials.SSID = CLOUD_WIFI_AP,
		.ap_credentials.password = CLOUD_WIFI_PW,
		.ap_credentials.security = CLOUD_WIFI_SECURITY,
		.BSSID = {0},
		.band = CLOUD_WIFI_BAND,
	};
	cy_wcm_config_t config = {.interface = CY_WCM_INTERFACE_TYPE_STA}; // We are a station (not a Access Point)

	cy_wcm_init(&config); // Initialize the connection manager

	printf("\nWi-Fi Connection Manager initialized.\n");

	do
	{
		cy_wcm_ip_address_t ip_address;

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
		else
		{
			printf("WiFi Connect Failed Retrying\n");
			vTaskDelay(2000); // wait 2 seconds and try again;
		}

	} while (result != CY_RSLT_SUCCESS);
}
