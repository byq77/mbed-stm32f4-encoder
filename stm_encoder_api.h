#ifndef __STM_ENCODER_API_H__
#define __STM_ENCODER_API_H__

#include "device.h"
#include "pinmap.h"

#if DEVICE_ENCODER
#include "objects_encoder.h"

typedef struct encoder_s encoder_t;

typedef struct {
    int peripheral;
    PinName channel_a;
    int channel_a_function;
    PinName channel_b;
    int channel_b_function;
    bool polarity;
}encoder_pinmap_t;

#ifdef __cplusplus
extern "C" {
#endif

void encoder_init_direct(encoder_t *obj, const encoder_pinmap_t *pinmap);

int32_t encoder_get_count(encoder_t *obj);

bool encoder_get_dir(encoder_t *obj);

void encoder_reset(encoder_t *obj);

void encoder_start(encoder_t *obj);

void encoder_stop(encoder_t *obj);

void encoder_free(encoder_t *obj);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* DEVICE_ENCODER */

#endif /* __STM_ENCODER_API_H__ */