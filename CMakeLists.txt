cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico2_w)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(morseMachine C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PICO_MBEDTLS_CONFIG_FILE "${CMAKE_CURRENT_LIST_DIR}/http/mbedtls_config.h" CACHE STRING "" FORCE)

pico_sdk_init()

include(FetchContent)

FetchContent_Declare(
        ArduinoJson
        GIT_REPOSITORY https://github.com/bblanchon/ArduinoJson.git
        GIT_TAG        v7.4.3
)

FetchContent_MakeAvailable(ArduinoJson)

add_executable(morseMachine
        main.cpp
        partPack/OledPack.cpp
        lib/ssd1306.c
        http/mbedtls_config.h
        http/HttpClient.cpp
        layout/Page.h
        layout/PageStack.cpp
        layout/PageStack.h
        partPack/ButtonPack.cpp
        layout/page/MenuPage.cpp
        layout/page/MenuPage.h
        layout/page/InputPage.cpp
        layout/page/InputPage.h
        layout/page/PageConstant.h
        partPack/Buzzer.h
        partPack/Joystick.h
        layout/OptionPage.h
        layout/page/GlobalState.h
        layout/page/RegisterPage.cpp
        layout/page/RegisterPage.h
        layout/net/RequestUtil.h
        util/WifiManager.h
        layout/HintOptionsPage.h
        layout/page/LoginPage.cpp
        layout/page/LoginPage.h
        layout/page/SendMorsePage.cpp
        layout/page/SendMorsePage.h
        layout/page/InputCodePage.cpp
        layout/page/InputCodePage.h
        layout/page/MessageListPage.cpp
        layout/page/MessageListPage.h
        layout/page/MorseListenPage.cpp
        layout/page/MorseListenPage.h
        layout/page/WifiListPage.cpp
        layout/page/WifiListPage.h
        layout/page/WifiConnectPage.cpp
        layout/page/WifiConnectPage.h
)

target_include_directories(morseMachine PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        http
        lib
)

target_link_libraries(morseMachine
        pico_stdlib
        pico_multicore
        hardware_pwm
        hardware_i2c
        hardware_adc
        hardware_gpio
        pico_cyw43_arch_lwip_poll
        pico_lwip_sntp
        pico_mbedtls
        ArduinoJson
)

pico_enable_stdio_usb(morseMachine 1)
pico_enable_stdio_uart(morseMachine 0)

pico_add_extra_outputs(morseMachine)