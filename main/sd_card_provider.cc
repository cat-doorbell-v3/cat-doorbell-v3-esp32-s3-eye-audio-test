#include <dirent.h> // Directory Entry
#include "esp_log.h"

#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_default_configs.h"

#include "sd_card_provider.h"

#define TAG "SD_CARD_PROVIDER"

SDCardProvider* SDCardProvider::instance = nullptr;

SDCardProvider& SDCardProvider::getInstance(const char* mountPoint) {
    if (instance == nullptr) {
        instance = new SDCardProvider(mountPoint);
    }
    return *instance;
}

SDCardProvider::SDCardProvider(const char* mountPoint)
    : mountPoint(mountPoint), card(nullptr) {
    // Constructor logic here (if necessary)
}

SDCardProvider::~SDCardProvider() {
    // Destructor logic, e.g., unmounting the SD card
    if (card) {
        esp_vfs_fat_sdcard_unmount(mountPoint, card);
        ESP_LOGI(TAG, "SD card unmounted");
    }
    instance = nullptr;
}

esp_err_t SDCardProvider::initialize() {
    ESP_LOGI(TAG, "Initializing SD card");

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.flags = SDMMC_HOST_FLAG_1BIT; // Specify the 1-bit mode

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.width = 1; // Set the slot width to 1-bit
    // Manually configure the GPIOs used for SDMMC interface
    slot_config.clk = (gpio_num_t)39;
    slot_config.cmd = (gpio_num_t)38;
    slot_config.d0 = (gpio_num_t)40;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 32 * 1024
    };

    ESP_LOGI(TAG, "Mounting SD card");
    esp_err_t ret = esp_vfs_fat_sdmmc_mount(mountPoint, &host, &slot_config, &mount_config, &card);

    ESP_LOGI(TAG, "Checking mount status");
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. If you want the card to be formatted, set format_if_mount_failed = true.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return ret;
    }

    ESP_LOGI(TAG, "SD card mounted successfully");
    return ESP_OK;
}

void SDCardProvider::listDir(const char* path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        ESP_LOGE(TAG, "Failed to open directory %s", path);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Check if it's a directory or a file
        if (entry->d_type == DT_DIR) {
            ESP_LOGI(TAG, "Directory: %s", entry->d_name);
        } else {
            ESP_LOGI(TAG, "File: %s", entry->d_name);
        }
    }
    closedir(dir);
}