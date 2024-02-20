idf_component_register(
    SRCS 
      main.cpp  # Your main application source file
      cat_sounds.cc  # Your TFLite model in hex format
    PRIV_REQUIRES driver freertos 
    INCLUDE_DIRS "")


# If TensorFlow Lite for Microcontrollers is a separate component, you might need to add it to the REQUIRES list
# or use target_link_libraries(${COMPONENT_LIB} PRIVATE tflite_micro) if it's set up as a library
target_link_libraries(${COMPONENT_LIB} PRIVATE tflite_micro)

