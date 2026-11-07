#include "hal/hal_storage.h"
#include <string.h>

#define MOCK_MAX_FILES 8
#define MOCK_FILE_MAX_SIZE 2048
#define MOCK_MAX_PATH_LEN 64

typedef struct {
    char path[MOCK_MAX_PATH_LEN];
    uint8_t data[MOCK_FILE_MAX_SIZE];
    size_t size;
    bool in_use;
    hal_storage_partition_t part;
} mock_file_entry_t;

static const hal_storage_driver_t *s_active_driver = NULL;
static bool s_mock_ready = false;
static bool s_mounted[4] = {false, false, false, false};
static mock_file_entry_t s_mock_fs[MOCK_MAX_FILES];

static nova_status_t mock_storage_init(void) {
    s_mock_ready = true;
    for (int i = 0; i < 4; i++) {
        s_mounted[i] = false;
    }
    for (int i = 0; i < MOCK_MAX_FILES; i++) {
        s_mock_fs[i].in_use = false;
    }
    return NOVA_SUCCESS;
}

static nova_status_t mock_storage_mount(hal_storage_partition_t part) {
    if (part > HAL_STORAGE_PARTITION_CONFIG) return NOVA_ERROR_INVALID_PARAM;
    s_mounted[part] = true;
    return NOVA_SUCCESS;
}

static nova_status_t mock_storage_unmount(hal_storage_partition_t part) {
    if (part > HAL_STORAGE_PARTITION_CONFIG) return NOVA_ERROR_INVALID_PARAM;
    s_mounted[part] = false;
    return NOVA_SUCCESS;
}

static nova_status_t mock_storage_read_file(hal_storage_partition_t part, const char *path, void *buffer, size_t size, size_t *out_read) {
    if (!s_mock_ready) return NOVA_ERROR_NOT_INITIALIZED;
    if (part > HAL_STORAGE_PARTITION_CONFIG || !s_mounted[part]) return NOVA_ERROR_INVALID_STATE;
    if (!path || !buffer || !out_read) return NOVA_ERROR_INVALID_PARAM;

    for (int i = 0; i < MOCK_MAX_FILES; i++) {
        if (s_mock_fs[i].in_use && s_mock_fs[i].part == part && strcmp(s_mock_fs[i].path, path) == 0) {
            size_t to_copy = (size < s_mock_fs[i].size) ? size : s_mock_fs[i].size;
            memcpy(buffer, s_mock_fs[i].data, to_copy);
            *out_read = to_copy;
            return NOVA_SUCCESS;
        }
    }
    return NOVA_ERROR_NOT_FOUND;
}

static nova_status_t mock_storage_write_file(hal_storage_partition_t part, const char *path, const void *data, size_t size) {
    if (!s_mock_ready) return NOVA_ERROR_NOT_INITIALIZED;
    if (part > HAL_STORAGE_PARTITION_CONFIG || !s_mounted[part]) return NOVA_ERROR_INVALID_STATE;
    if (!path || !data || size > MOCK_FILE_MAX_SIZE) return NOVA_ERROR_INVALID_PARAM;

    /* Check if already exists */
    for (int i = 0; i < MOCK_MAX_FILES; i++) {
        if (s_mock_fs[i].in_use && s_mock_fs[i].part == part && strcmp(s_mock_fs[i].path, path) == 0) {
            memcpy(s_mock_fs[i].data, data, size);
            s_mock_fs[i].size = size;
            return NOVA_SUCCESS;
        }
    }

    /* Find empty slot */
    for (int i = 0; i < MOCK_MAX_FILES; i++) {
        if (!s_mock_fs[i].in_use) {
            s_mock_fs[i].in_use = true;
            s_mock_fs[i].part = part;
            strncpy(s_mock_fs[i].path, path, MOCK_MAX_PATH_LEN - 1);
            s_mock_fs[i].path[MOCK_MAX_PATH_LEN - 1] = '\0';
            memcpy(s_mock_fs[i].data, data, size);
            s_mock_fs[i].size = size;
            return NOVA_SUCCESS;
        }
    }
    return NOVA_ERROR_NO_MEMORY;
}

static nova_status_t mock_storage_delete_file(hal_storage_partition_t part, const char *path) {
    if (!s_mock_ready) return NOVA_ERROR_NOT_INITIALIZED;
    if (part > HAL_STORAGE_PARTITION_CONFIG || !s_mounted[part]) return NOVA_ERROR_INVALID_STATE;
    if (!path) return NOVA_ERROR_INVALID_PARAM;

    for (int i = 0; i < MOCK_MAX_FILES; i++) {
        if (s_mock_fs[i].in_use && s_mock_fs[i].part == part && strcmp(s_mock_fs[i].path, path) == 0) {
            s_mock_fs[i].in_use = false;
            return NOVA_SUCCESS;
        }
    }
    return NOVA_ERROR_NOT_FOUND;
}

static nova_status_t mock_storage_get_info(hal_storage_partition_t part, hal_storage_info_t *info) {
    if (!info) return NOVA_ERROR_INVALID_PARAM;
    if (part > HAL_STORAGE_PARTITION_CONFIG) return NOVA_ERROR_INVALID_PARAM;

    info->total_space_bytes = 4 * 1024 * 1024; // 4MB per partition
    info->free_space_bytes = 3 * 1024 * 1024;
    info->is_mounted = s_mounted[part];
    return NOVA_SUCCESS;
}

static bool mock_storage_is_ready(void) {
    return s_mock_ready;
}

static const hal_storage_driver_t s_mock_driver = {
    .init = mock_storage_init,
    .mount = mock_storage_mount,
    .unmount = mock_storage_unmount,
    .read_file = mock_storage_read_file,
    .write_file = mock_storage_write_file,
    .delete_file = mock_storage_delete_file,
    .get_info = mock_storage_get_info,
    .is_ready = mock_storage_is_ready
};

const hal_storage_driver_t* hal_storage_get_mock_driver(void) {
    return &s_mock_driver;
}

void hal_storage_register_driver(const hal_storage_driver_t *driver) {
    s_active_driver = driver;
}

nova_status_t hal_storage_init(void) {
    if (!s_active_driver) {
        s_active_driver = &s_mock_driver;
    }
    return s_active_driver->init();
}

nova_status_t hal_storage_mount(hal_storage_partition_t part) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->mount(part);
}

nova_status_t hal_storage_unmount(hal_storage_partition_t part) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->unmount(part);
}

nova_status_t hal_storage_read_file(hal_storage_partition_t part, const char *path, void *buffer, size_t size, size_t *out_read) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->read_file(part, path, buffer, size, out_read);
}

nova_status_t hal_storage_write_file(hal_storage_partition_t part, const char *path, const void *data, size_t size) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->write_file(part, path, data, size);
}

nova_status_t hal_storage_delete_file(hal_storage_partition_t part, const char *path) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->delete_file(part, path);
}

nova_status_t hal_storage_get_info(hal_storage_partition_t part, hal_storage_info_t *info) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->get_info(part, info);
}

bool hal_storage_is_ready(void) {
    if (!s_active_driver) return false;
    return s_active_driver->is_ready();
}
