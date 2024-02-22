idf_component_register(
    SRCS 
      main.cpp  # Your main application source file
      cat_sounds.c  # Your TFLite model in hex format
    PRIV_REQUIRES driver freertos 
    INCLUDE_DIRS ".")
