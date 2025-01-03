target_sources(${NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/unsafe_fifo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/byte_fifo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/csal_lwip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/local_eventqueue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lp_workqueue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/event_management.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os_error.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os_cli.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os_quick_fft.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/safe_circular_queue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/safe_fifo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thread_init.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/statemachine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/trig_lut.cpp
)
