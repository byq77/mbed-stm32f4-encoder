#include "mbed_assert.h"
#include "mbed_error.h"
#include "mbed_debug.h"
#include "mbed_critical.h"
#include "mbed_wait_api.h"
#include "stm_encoder_api.h"

#if DEVICE_ENCODER

#include "stm32f4xx_hal.h"

#define TIM_INST(obj) ((TIM_TypeDef *)(obj->etim))

#define MAX_TIMER_VALUE_HALF 32768

#define NVIC_SETUP(INTERRUPT)                                         \
    do                                                                \
    {                                                                 \
        HAL_NVIC_SetPriority(INTERRUPT, 6, 0);                        \
        HAL_NVIC_EnableIRQ(INTERRUPT);                                \
    } while (0)

TIM_HandleTypeDef htim_enc[MAX_HTIM];
volatile int32_t hbits_enc[MAX_HTIM];

static void encoder_update_hbits_irq(TIM_HandleIndex itim)
{
    TIM_HandleTypeDef * htim = &htim_enc[itim];

    /* TIM Update event */
    if (__HAL_TIM_GET_FLAG(htim, TIM_FLAG_UPDATE) != RESET)
    {
        if (__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_UPDATE) != RESET)
        {
            __HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);

            if (__HAL_TIM_IS_TIM_COUNTING_DOWN(htim))
                hbits_enc[itim] -= 1;
            else
                hbits_enc[itim] += 1;
        }
    }
}

static TIM_HandleIndex getTIM_HandleIndex(EncoderName name)
{
#if defined(TIM1_BASE)
    if(name == ENCODER_TIM1)
        return HTIM1;
#endif
#if defined(TIM2_BASE)
    if(name == ENCODER_TIM2)
        return HTIM2;
#endif
#if defined(TIM3_BASE)
    if(name == ENCODER_TIM3)
        return HTIM3;
#endif
#if defined(TIM4_BASE)
    if(name == ENCODER_TIM4)
        return HTIM4;
#endif
#if defined(TIM8_BASE)
    if(name == ENCODER_TIM8)
        return HTIM8;
#endif
}

#if defined(TIM1_BASE)
void TIM1_UP_TIM10_IRQHandler(void)
{
    encoder_update_hbits_irq(HTIM1);
}
#endif
#if defined(TIM2_BASE)
void TIM2_IRQHandler(void)
{
    encoder_update_hbits_irq(HTIM2);
}
#endif
#if defined(TIM3_BASE)
void TIM3_IRQHandler(void)
{
    encoder_update_hbits_irq(HTIM3);
}
#endif
#if defined(TIM4_BASE)
void TIM4_IRQHandler(void)
{
    encoder_update_hbits_irq(HTIM4);
}
#endif
#if defined(TIM8_BASE)
void TIM8_UP_TIM13_IRQHandler(void)
{
    encoder_update_hbits_irq(HTIM8);
}
#endif

void encoder_init_direct(encoder_t *obj, const encoder_pinmap_t *pinmap)
{
    obj->etim = (EncoderName)pinmap->peripheral;
    MBED_ASSERT(obj->etim != (EncoderName)NC);

    obj->itim = getTIM_HandleIndex(obj->etim);

    TIM_HandleTypeDef * htim = &htim_enc[obj->itim];
    TIM_Encoder_InitTypeDef sConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    // Enable TIM clock
#if defined(TIM1_BASE)
    if(obj->etim == ENCODER_TIM1)
    {
        __HAL_RCC_TIM1_CLK_ENABLE();
    }
#endif
#if defined(TIM2_BASE)
    if(obj->etim == ENCODER_TIM2)
    {
        __HAL_RCC_TIM2_CLK_ENABLE();
    }
#endif
#if defined(TIM3_BASE)
    if(obj->etim == ENCODER_TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();
    }
#endif
#if defined(TIM4_BASE)
    if(obj->etim == ENCODER_TIM4)
    {
        __HAL_RCC_TIM4_CLK_ENABLE();
    }
#endif
#if defined(TIM8_BASE)
    if(obj->etim == ENCODER_TIM8)
    {
        __HAL_RCC_TIM8_CLK_ENABLE();
    }
#endif

    // Configure GPIOs
    pin_function(pinmap->channel_a, pinmap->channel_a_function);
    pin_function(pinmap->channel_b, pinmap->channel_b_function);

    obj->channel_a = pinmap->channel_a;
    obj->channel_b = pinmap->channel_b;

    // Configure NVIC
#if defined(TIM1_BASE)
    if(obj->etim == ENCODER_TIM1)
    {
        NVIC_SETUP(TIM1_UP_TIM10_IRQn);
    }
#endif
#if defined(TIM2_BASE)
    if(obj->etim == ENCODER_TIM2)
    {
        NVIC_SETUP(TIM2_IRQn);
    }
#endif
#if defined(TIM3_BASE)
    if(obj->etim == ENCODER_TIM3)
    {
        NVIC_SETUP(TIM3_IRQn);
    }
#endif
#if defined(TIM4_BASE)
    if(obj->etim == ENCODER_TIM4)
    {
        NVIC_SETUP(TIM4_IRQn);
    }
#endif
#if defined(TIM8_BASE)
    if(obj->etim == ENCODER_TIM8)
    {
        NVIC_SETUP(TIM8_UP_TIM13_IRQn);
    }
#endif

    // Configure timer
    htim->Instance = TIM_INST(obj);
    htim->Init.Prescaler = 0x0;
    htim->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim->Init.Period = 0xffff;
    htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
    if(pinmap->polarity)
        sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
    else
        sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
    sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC1Filter = 0xf;
    sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
    sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC2Filter = 0xf;

    if (HAL_TIM_Encoder_Init(htim, &sConfig) != HAL_OK)
    {
        error("Cannot initialize encoder interface");
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig) != HAL_OK)
    {
        error("Cannot configure timer in master mode");
    }
} 

void encoder_start(encoder_t *obj)
{
    TIM_HandleTypeDef * htim = &htim_enc[obj->itim];;

    if (HAL_TIM_Encoder_Start(htim, TIM_CHANNEL_ALL) != HAL_OK)
    {
        error("Cannot start encoder");
    }
    
    // Update (aka over- and underflow) interrupt enabled
    __HAL_TIM_ENABLE_IT(htim, TIM_IT_UPDATE);
    // The initialisation process generates an update interrupt, so we'll have to clear the update flag before anything else
    __HAL_TIM_CLEAR_FLAG(htim, TIM_IT_UPDATE);        
}

void encoder_stop(encoder_t *obj)
{
    TIM_HandleTypeDef * htim = &htim_enc[obj->itim];;

    if (HAL_TIM_Encoder_Stop(htim, TIM_CHANNEL_ALL) != HAL_OK)
    {
        error("Cannot stop encoder");
    }

    __HAL_TIM_DISABLE_IT(htim, TIM_IT_UPDATE);
    __HAL_TIM_CLEAR_FLAG(htim, TIM_IT_UPDATE);        
}

int32_t encoder_get_count(encoder_t * obj)
{
    TIM_HandleTypeDef * htim = &htim_enc[obj->itim];
    int32_t count, hbits;
    count = __HAL_TIM_GET_COUNTER(htim);
    /* TIM Update event */
    if (__HAL_TIM_GET_FLAG(htim, TIM_FLAG_UPDATE) != RESET)
    {
        if (__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_UPDATE) != RESET)
        {
            __HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);
            if (count < MAX_TIMER_VALUE_HALF)
                hbits_enc[obj->itim] += 1;
            else
                hbits_enc[obj->itim] -= 1;
        }
    }
    hbits = hbits_enc[obj->itim];
    return (hbits << 16) | count;
}

void encoder_reset(encoder_t *obj)
{
    TIM_HandleTypeDef * htim = &htim_enc[obj->itim];;
    __HAL_TIM_SET_COUNTER(htim, 0);
    hbits_enc[obj->itim] = 0;
}

bool encoder_get_dir(encoder_t *obj)
{
    TIM_HandleTypeDef * htim = &htim_enc[obj->itim];;
    return __HAL_TIM_IS_TIM_COUNTING_DOWN(htim);
}

void encoder_free(encoder_t *obj)
{
    TIM_HandleTypeDef * htim = &htim_enc[obj->itim];;
    HAL_TIM_Encoder_DeInit(htim);

#if defined(TIM1_BASE)
    if(obj->etim == ENCODER_TIM1)
    {
        __HAL_RCC_TIM1_CLK_DISABLE();
        HAL_NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);
    }
#endif
#if defined(TIM2_BASE)
    if(obj->etim == ENCODER_TIM2)
    {
        __HAL_RCC_TIM2_CLK_DISABLE();
        HAL_NVIC_DisableIRQ(TIM2_IRQn);
    }
#endif
#if defined(TIM3_BASE)
    if(obj->etim == ENCODER_TIM3)
    {
        __HAL_RCC_TIM3_CLK_DISABLE();
        HAL_NVIC_DisableIRQ(TIM3_IRQn);
    }
#endif
#if defined(TIM4_BASE)
    if(obj->etim == ENCODER_TIM4)
    {
        __HAL_RCC_TIM4_CLK_DISABLE();
        HAL_NVIC_DisableIRQ(TIM4_IRQn);
    }
#endif
#if defined(TIM8_BASE)
    if(obj->etim == ENCODER_TIM8)
    {
        __HAL_RCC_TIM8_CLK_DISABLE();
        HAL_NVIC_DisableIRQ(TIM8_UP_TIM13_IRQn);
    }
#endif

    // Configure GPIOs back to reset value
    pin_function(obj->channel_a, STM_PIN_DATA(STM_MODE_ANALOG, GPIO_NOPULL, 0));
    pin_function(obj->channel_b, STM_PIN_DATA(STM_MODE_ANALOG, GPIO_NOPULL, 0));
}

#endif /* DEVICE_ENCODER */