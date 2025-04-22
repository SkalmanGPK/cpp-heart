# cpp-heart
Heart going from left to right. Inspired by the arduino Uno R4 wifi.
The code utilizes three 32 bit integers to display the matrix, same as the arduino does. But it translates it temporarily to a flat 96-bit vector.
this makes it easier to circular bitshift, and manipulate how the bits should be shifted.
