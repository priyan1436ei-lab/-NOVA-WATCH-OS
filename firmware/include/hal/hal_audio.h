#ifndef HAL_AUDIO_H
#define HAL_AUDIO_H

#include "hal/hal_common.h"
#include "nova_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*hal_audio_pcm_cb_t)(const int16_t *samples, size_t sample_count, void *user_data);

typedef struct {
    nova_status_t (*init)(void);
    nova_status_t (*start_mic_capture)(hal_audio_pcm_cb_t cb, void *user_data);
    nova_status_t (*stop_mic_capture)(void);
    nova_status_t (*play_pcm)(const int16_t *samples, size_t sample_count);
    nova_status_t (*set_speaker_volume)(uint8_t volume_pct);
    bool          (*is_ready)(void);
} hal_audio_driver_t;

nova_status_t hal_audio_init(void);
nova_status_t hal_audio_start_mic_capture(hal_audio_pcm_cb_t cb, void *user_data);
nova_status_t hal_audio_stop_mic_capture(void);
nova_status_t hal_audio_play_pcm(const int16_t *samples, size_t sample_count);
nova_status_t hal_audio_set_speaker_volume(uint8_t volume_pct);
bool          hal_audio_is_ready(void);

void hal_audio_register_driver(const hal_audio_driver_t *driver);
const hal_audio_driver_t* hal_audio_get_mock_driver(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_AUDIO_H */

