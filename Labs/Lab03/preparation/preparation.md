1. Calculate the overflow times for three Timer/Counter modules that contain ATmega328P if CPU clock frequency is 16 MHz. Complete the following table for given prescaler values. Note that, Timer/Counter2 is able to set 7 prescaler values, including 32 and 128 and other timers have only 5 prescaler values.

    | **Module** | **Number of bits** |**1** | **8** |**32** |**64** |**128** |**256** |**1024** |
    | :-: | :-: | :-: | :-: |:-: |:-: |:-: |:-: |:-: |
    | Timer/Counter0 | 8 | 16u | 128u | -- |1m |-- |4.1m |16.4m |
    | Timer/Counter1 | 16|4.1m  |32.8m  |-- |262.1m |-- |1.49 |4.194 |
    | Timer/Counter2 | 8 | 16u | 128u | 512u|1m |2m |4.1m |16.4m |
