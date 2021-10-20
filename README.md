# Audio_SPI_Export_Tool

芯片平台：STM32H743VIT6

操作系统：FreeRTOS V10.3.1

功能作用：虚拟音频输出

通过算法生成正弦音频，输出到SPI1总线，由从机接收，输出到USB Audio Microphone

注意点：

不开启MPU、不开器DCache

DMA数据放置0x38000000区域