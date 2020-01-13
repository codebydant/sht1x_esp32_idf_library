# SHT1X LIBRARY FOR ESP-IDF Programming
This is a C library for the SHT1X Temperature-Humidity sensor for esp-idf framework

This code is taken from: sht1x-library-Arduino and https://github.com/practicalarduino/SHT1x, and it was modified for the esp-idf framework https://github.com/sanket2994/sht1x-library-Arduino

<img src="./pins-interface/sht15.png" align="center" height="500" width="900"><br>

## How to compile

    $ git clone https://github.com/danielTobon43/sht1x_esp32_idf_library.git
    $ idf.py build
    $ idf.py -p <port number> -b <baud frenq> flash monitor
     
     e.g. 
     idf.py build && idf.py -p /dev/ttyUSB0 -b 115200 flash monitor
     
     Output example:     
     ~~~~~~~~~~~~~~~~~
      SHT1X Sensor   
    ~~~~~~~~~~~~~~~~~
    -> setting data pin 27 
    -> setting sck pin 2 

    ***************************
    -> Temperature:  21.11 °C 
    -> Temperature:  70.03 F 
    -> Humidity: -12.09% 

    -> Temperature:  21.10 °C 
    -> Temperature:  70.02 F 
    -> Humidity: -12.09% 

    
 ## How to include in your own project
 
    1.  copy sht1x.c in your src folder
    2.  copy sht1x.h in your include folder
    3.  set the sht1x.c in your CMakeLists.txt on your main folder
    
    e.g.
    
      set(srcs  "src/main.c" 
                "src/sht1x.c"
       )
       
       idf_component_register(SRCS ${srcs} INCLUDE_DIRS "." )
       
    4.  set the include directory on your top CMakeLists.txt on your root project
    
    e.g.
    
      include($ENV{IDF_PATH}/tools/cmake/project.cmake)    
      include_directories ("${CMAKE_SOURCE_DIR}/main/include")
      
    5.  set the include header on your main program
    
    e.g.
    
      #include "include/sht1x.h"
      
   ## Example
   <img src="./example/example1.png" align="center" height="850" width="600"><br>
      
   
    


     


