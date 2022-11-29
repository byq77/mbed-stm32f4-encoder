#ifndef __OBJECTS_ENCODER_H__
#define __OBJECTS_ENCODER_H__

#include "cmsis.h"
#include "PortNames.h"
#include "PeripheralNames.h"
#include "PinNames.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(TARGET_CORE2)

typedef enum {
    ENCODER_TIM1 = (int)TIM1_BASE,
    ENCODER_TIM2 = (int)TIM2_BASE,
    ENCODER_TIM8 = (int)TIM8_BASE,
    ENCODER_TIM3 = (int)TIM3_BASE,
    ENCODER_TIM4 = (int)TIM4_BASE
} EncoderName;

typedef enum {
    HTIM1 = 0,
    HTIM2,
    HTIM3,
    HTIM4,
    HTIM8,
    MAX_HTIM
} TIM_HandleIndex;

#else

#error "Target is not supported!"

#endif /* TARGET_CORE2 */

struct encoder_s {
    TIM_HandleIndex itim;
    EncoderName etim;
    PinName channel_a;
    PinName channel_b;
};

#ifdef __cplusplus
}
#endif

#endif /* __OBJECTS_ENCODER_H__ */