# STM32L476 DISCO USB BMP Viewer

------

Images available on USB Stick are displayed sequentially in a forever loop.

In this case, format the SD card and put into its root directory the .bmp be displayed on TFT mentioning that it is not supported.In this case, put into the USB Stick root directory the .bmp files available within the FW package under Utilities\Media\Images folder which are respecting the following criteria:

   o Dimensions: 240x320
   o Width: 240 pixels
   o Height: 320 pixels
   o Bit depth: 16 or 24
   o Item type: BMP file
   o The name of the bmp image file can exceed 11 characters but not recommend (including.bmp extension).
     
![](https://raw.githubusercontent.com/nickfox-taterli/stm32l4_bmp_viewer/master/Screenshot/20170724101815.png)

![](https://raw.githubusercontent.com/nickfox-taterli/stm32l4_bmp_viewer/master/Screenshot/20170724101838.png)
