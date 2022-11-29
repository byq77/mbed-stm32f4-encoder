add_library(encoder-stm-mbed-lib INTERFACE)

target_include_directories(encoder-stm-mbed-lib
    INTERFACE
    .
)
    
target_sources(encoder-stm-mbed-lib
    INTERFACE
    stm_encoder_api.c
    Encoder.cpp
)