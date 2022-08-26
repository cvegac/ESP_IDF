idf_component_register(
    SRCS main.cc main_functions.cc audio_provider.cc feature_provider.cc
         no_micro_features_data.cc yes_micro_features_data.cc
         model.cc recognize_commands.cc command_responder.cc
         micro_features_generator.cc micro_model_settings.cc ringbuf.c
    PRIV_REQUIRES tflite-lib spi_flash
    INCLUDE_DIRS "")
