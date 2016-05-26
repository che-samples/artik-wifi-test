#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <artik_module.h>
#include <artik_platform.h>
#define AP_TO_CONNECT   "<put a SSID name here>"
#define AP_PASSWORD     "<put the password of the SSID here>"
artik_error test_wifi_scan()
{
    artik_wifi_module* wifi = (artik_wifi_module*)artik_get_api_module("wifi");
    artik_error ret;
    artik_wifi_ap *wifi_aps;
    int num_aps = 10, i = 0;
    wifi_aps = (artik_wifi_ap*)malloc(num_aps*sizeof(artik_wifi_ap));
    fprintf(stdout, "TEST: %s\n", __func__);
    /* Scan for APs and list their characteristics */
    ret = wifi->scan(wifi_aps, &num_aps);
    if (ret == S_OK) {
        for (i=0; i<num_aps; i++) {
            fprintf(stdout, "%s\t%d\t%d\t",
                    wifi_aps[i].bssid,
                    wifi_aps[i].frequency,
                    wifi_aps[i].signal_level);
            if (wifi_aps[i].encryption_flags & WIFI_ENCRYPTION_OPEN)
                fprintf(stdout, "[OPEN]");
            if (wifi_aps[i].encryption_flags & WIFI_ENCRYPTION_WEP)
                fprintf(stdout, "[WEP]");
            if (wifi_aps[i].encryption_flags & WIFI_ENCRYPTION_WPA)
                fprintf(stdout, "[WPA]");
            if (wifi_aps[i].encryption_flags & WIFI_ENCRYPTION_WPA2)
                fprintf(stdout, "[WPA2]");
            if (wifi_aps[i].encryption_flags & WIFI_ENCRYPTION_WPA2_PERSONAL)
                fprintf(stdout, "[PERSONAL]");
            if (wifi_aps[i].encryption_flags & WIFI_ENCRYPTION_WPA2_ENTERPRISE)
                fprintf(stdout, "[ENTERPRISE]");
            fprintf(stdout, "\t%s\n", wifi_aps[i].name);
        }
    } else {
        fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
        goto exit;
    }
    fprintf(stdout, "TEST: %s succeeded\n", __func__);
exit:
    free(wifi_aps);
    return ret;
}
artik_error test_wifi_connect()
{
    artik_wifi_module* wifi = (artik_wifi_module*)artik_get_api_module("wifi");
    artik_error ret = S_OK;
    artik_wifi_ap wifi_ap;
    fprintf(stdout, "TEST: %s\n", __func__);
    strncpy(wifi_ap.name, AP_TO_CONNECT, MAX_AP_NAME_LEN);
    wifi_ap.encryption_flags = WIFI_ENCRYPTION_WPA2|WIFI_ENCRYPTION_WPA2_PERSONAL;
    /* Connect to the AP */
    ret = wifi->connect(&wifi_ap, AP_PASSWORD, true);
    if (ret != S_OK) {
        fprintf(stderr, "TEST %s: Connection to AP %s failed (err=%d)\n", __func__, wifi_ap.name, ret);
        goto exit;
    }
    /* Wait for association to complete */
    ret = wifi->wait_for_connection(10000);
    if (ret != S_OK) {
        fprintf(stderr, "TEST %s: Failed to associate to AP %s (err=%d)\n", __func__, wifi_ap.name, ret);
        goto exit;
    }
    fprintf(stdout, "TEST: %s succeeded\n", __func__);
exit:
    return ret;
}
int main(void)
{
    artik_error ret = S_OK;
    ret = test_wifi_scan();
    ret = test_wifi_connect();
    return (ret == S_OK) ? 0 : -1;
}
