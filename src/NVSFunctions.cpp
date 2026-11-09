#include "NVSFunctions.h"

#include "nvs_flash.h"




bool NVSClass::erase_nvs()
{

    ESP_ERROR_CHECK(nvs_flash_erase());
    nvs_flash_init();
    return true;
}



/**
 * Check if an update was in progress.
 * return false if not.
 */
bool NVSClass::check_updateInProgress()
{
    char fwUpdateInProgress[20];
    size_t sizeoffwUpdateInProgress = sizeof(fwUpdateInProgress);

    bool resultado = false;

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
         Serial.println("NVS will be erased!!!");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);

//    Serial.println("Reading Last update status " );

    if (err == ESP_OK)
    {
        err = nvs_get_str(my_handle, "fwUIP", fwUpdateInProgress, &sizeoffwUpdateInProgress);
        if (err == ESP_OK)
        {
            Serial.println("It seems there was an update in progress: " + String(fwUpdateInProgress));
            resultado = true;
        }
        if (err != ESP_ERR_NVS_NOT_FOUND)
        {
            Serial.println("Error reading fwUpdateInProgress data on NVS: " + String(err));
        }
    }

    nvs_close(my_handle);

    return resultado;
}

bool NVSClass::write_updateInProgress(uint8_t fwUpdateInProgress)
{
    // fwUpdateInProgress '0' for firmware Update '100' for UI update (U_FLASH/U_SPIFF)
    bool resultado = false;

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);

    if (err == ESP_OK)
    {
        Serial.println("Writing fwUpdateInProgress:");
        //err = nvs_set_u8(my_handle, "fwUIP", fwUpdateInProgress);
        resultado = true;
        err = nvs_set_str(my_handle, "fwUIP", "UIUpdate");
        if (err != ESP_OK)
        {
            Serial.printf("Error writing fwUpdateInProgress data on NVS: 0x%02x\n", err);
        }
    }

    nvs_commit(my_handle);
    nvs_close(my_handle);

    return resultado;
}

bool NVSClass::delete_updateInProgress()
{

    bool resultado = true;

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    Serial.println("Deleting fwUpdateInProgress data on NVS...");
    if (err == ESP_OK)
    {
        err = nvs_erase_key(my_handle, "fwUIP");
        if (err != ESP_OK)
        {
            Serial.printf("Error deleting fwUpdateInProgress data on NVS: 0x%02x\n", err);
            return false;
        }
    }

    nvs_commit(my_handle);
    nvs_close(my_handle);

    return resultado;
}

