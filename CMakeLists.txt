cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(app)

zephyr_include_directories(${ZEPHYR_BASE})
zephyr_include_directories(include)

target_sources(app PRIVATE src/main.c)
target_sources_ifdef(CONFIG_ADC app PRIVATE src/adc.c)
