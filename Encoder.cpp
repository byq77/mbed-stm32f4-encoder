#include "Encoder.h"

#if DEVICE_ENCODER

#include "platform/mbed_critical.h"
#include "platform/mbed_power_mgmt.h"
#include "platform/mbed_assert.h"

Encoder::Encoder(const encoder_pinmap_t &pinmap)
{
    core_util_critical_section_enter();
    encoder_init_direct(&_encoder, &pinmap);
    core_util_critical_section_exit();
}

Encoder::~Encoder()
{
    core_util_critical_section_enter();
    encoder_free(&_encoder);
    core_util_critical_section_exit();
}

int32_t Encoder::getCount()
{
    core_util_critical_section_enter();
    int32_t val = encoder_get_count(&_encoder);
    core_util_critical_section_exit();
    return val;
}

void Encoder::resetCount()
{
    core_util_critical_section_enter();
    encoder_reset(&_encoder);
    core_util_critical_section_exit();
}

void Encoder::start()
{
    core_util_critical_section_enter();
    encoder_start(&_encoder);
    core_util_critical_section_exit();
}

void Encoder::stop()
{
    core_util_critical_section_enter();
    encoder_stop(&_encoder);
    core_util_critical_section_exit();
}

#endif /* __ENCODER_H__ */