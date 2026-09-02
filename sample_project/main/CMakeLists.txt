idf_component_register(SRCS "radar.c" "main.c" "alarm.c" "circular_buffer.c" "master_i2c.c" "radar.c"
                    INCLUDE_DIRS "."
                    REQUIRES
                        esp_driver_gptimer
                        esp_driver_i2c
                        )
