#include "hal/hal_audio.h"

static const hal_audio_driver_t *s_active_driver = NULL;
static bool s_mock_ready = false;
static bool s_mock_capturing = false;
static uint8_t s_mock_volume = 80;

static nova_status_t mock_audio_init(void) {
    s_mock_ready = true;
    s_mock_capturing = false;
    s_mock_volume = 80;
    return NOVA_SUCCESS;
}

static nova_status_t mock_audio_start_mic_capture(hal_audio_pcm_cb_t cb, void *user_data) {
    (void)cb;
    (void)user_data;
    if (!s_mock_ready) return NOVA_ERROR_NOT_INITIALIZED;
    s_mock_capturing = true;
    return NOVA_SUCCESS;
}

static nova_status_t mock_audio_stop_mic_capture(void) {
    s_mock_capturing = false;
    return NOVA_SUCCESS;
}

static nova_status_t mock_audio_play_pcm(const int16_t *samples, size_t sample_count) {
    (void)samples;
    (void)sample_count;
    if (!s_mock_ready) return NOVA_ERROR_NOT_INITIALIZED;
    return NOVA_SUCCESS;
}

static nova_status_t mock_audio_set_speaker_volume(uint8_t volume_pct) {
    if (volume_pct > 100) return NOVA_ERROR_INVALID_PARAM;
    s_mock_volume = volume_pct;
    return NOVA_SUCCESS;
}

static bool mock_audio_is_ready(void) {
    return s_mock_ready;
}

static const hal_audio_driver_t s_mock_driver = {
    .init = mock_audio_init,
    .start_mic_capture = mock_audio_start_mic_capture,
    .stop_mic_capture = mock_audio_stop_mic_capture,
    .play_pcm = mock_audio_play_pcm,
    .set_speaker_volume = mock_audio_set_speaker_volume,
    .is_ready = mock_audio_is_ready
};

const hal_audio_driver_t* hal_audio_get_mock_driver(void) {
    return &s_mock_driver;
}

void hal_audio_register_driver(const hal_audio_driver_t *driver) {
    s_active_driver = driver;
}

nova_status_t hal_audio_init(void) {
    if (!s_active_driver) {
        s_active_driver = &s_mock_driver;
    }
    return s_active_driver->init();
}

nova_status_t hal_audio_start_mic_capture(hal_audio_pcm_cb_t cb, void *user_data) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->start_mic_capture(cb, user_data);
}

nova_status_t hal_audio_stop_mic_capture(void) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->stop_mic_capture();
}

nova_status_t hal_audio_play_pcm(const int16_t *samples, size_t sample_count) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->play_pcm(samples, sample_count);
}

nova_status_t hal_audio_set_speaker_volume(uint8_t volume_pct) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->set_speaker_volume(volume_pct);
}

bool hal_audio_is_ready(void) {
    if (!s_active_driver) return false;
    return s_active_driver->is_ready();
}

