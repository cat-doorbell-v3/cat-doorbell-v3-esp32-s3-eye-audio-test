// In sd_card_provider.h
#ifndef SD_CARD_PROVIDER_H
#define SD_CARD_PROVIDER_H

#include "esp_err.h"
#include "sdmmc_cmd.h"


#define MOUNT_POINT "/sdcard"

class SDCardProvider {
public:
    static SDCardProvider& getInstance(const char* mountPoint = MOUNT_POINT); 

    esp_err_t initialize();
    void listDir(const char* dirname);

    SDCardProvider(const SDCardProvider&) = delete;
    SDCardProvider& operator=(const SDCardProvider&) = delete;

private:
    SDCardProvider(const char* mountPoint);
    ~SDCardProvider();

    static SDCardProvider* instance; // Declare the static instance variable

    const char* mountPoint;
    sdmmc_card_t* card;
};

#endif // SD_CARD_PROVIDER_H
