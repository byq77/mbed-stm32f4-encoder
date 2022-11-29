#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "platform/platform.h"

#if DEVICE_ENCODER
#include "stm_encoder_api.h"

class Encoder {

public:
    Encoder(const encoder_pinmap_t &pinmap);
    Encoder(const encoder_pinmap_t &&) = delete;
    
    ~Encoder();

    /**
     * @brief Get current encoder count.
     * @return num of encoder ticks
     */
    int32_t getCount();

    /**
     * @brief Reset current encoder count.
     */
    void resetCount();

    /**
     * @brief Start encoder.
     */
    void start();
    
    /**
     * @brief Stop encoder.
     */
    void stop();

    operator int32_t()
    {
        return getCount();
    }

protected:

    encoder_t _encoder;
};

#endif

#endif /* __ENCODER_H__ */