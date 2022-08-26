idf_component_register(
    SRCS main.cc main_functions.cc constants.cc output_handler.cc model.cc
    PRIV_REQUIRES tflite-lib spi_flash
    INCLUDE_DIRS "")
