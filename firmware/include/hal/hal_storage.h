#ifndef HAL_STORAGE_H
#define HAL_STORAGE_H

#include "hal/hal_common.h"
#include "nova_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_STORAGE_PARTITION_RECOVERY = 0,
    HAL_STORAGE_PARTITION_FACES    = 1,
    HAL_STORAGE_PARTITION_HEALTH   = 2,
    HAL_STORAGE_PARTITION_CONFIG   = 3
} hal_storage_partition_t;

typedef struct {
    uint32_t total_space_bytes;
    uint32_t free_space_bytes;
    bool is_mounted;
} hal_storage_info_t;

typedef struct {
    nova_status_t (*init)(void);
    nova_status_t (*mount)(hal_storage_partition_t part);
    nova_status_t (*unmount)(hal_storage_partition_t part);
    nova_status_t (*read_file)(hal_storage_partition_t part, const char *path, void *buffer, size_t size, size_t *out_read);
    nova_status_t (*write_file)(hal_storage_partition_t part, const char *path, const void *data, size_t size);
    nova_status_t (*delete_file)(hal_storage_partition_t part, const char *path);
    nova_status_t (*get_info)(hal_storage_partition_t part, hal_storage_info_t *info);
    bool          (*is_ready)(void);
} hal_storage_driver_t;

nova_status_t hal_storage_init(void);
nova_status_t hal_storage_mount(hal_storage_partition_t part);
nova_status_t hal_storage_unmount(hal_storage_partition_t part);
nova_status_t hal_storage_read_file(hal_storage_partition_t part, const char *path, void *buffer, size_t size, size_t *out_read);
nova_status_t hal_storage_write_file(hal_storage_partition_t part, const char *path, const void *data, size_t size);
nova_status_t hal_storage_delete_file(hal_storage_partition_t part, const char *path);
nova_status_t hal_storage_get_info(hal_storage_partition_t part, hal_storage_info_t *info);
bool          hal_storage_is_ready(void);

void hal_storage_register_driver(const hal_storage_driver_t *driver);
const hal_storage_driver_t* hal_storage_get_mock_driver(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_STORAGE_H */
