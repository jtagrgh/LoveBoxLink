## LoveBox UNO R4
```
      🛜                                  🛜
  __________         _______         __________
 /________ /|       |_______|       |\ _________\
|         | | <---> |_______| <---> | |         |
|    <3   | |       |_____o_|       | |    <3   |
|_________|/        |_______|        \|_________| 
                    
```

## Description
A shared live blackboard for the Arduino R4. Draw a picture on one Arduino, and see it magically appear on another, anywhere in the world.

## Necessities
1. [Arduino R4](https://store-usa.arduino.cc/products/uno-r4-wifi?selectedStore=us) (1+)
2. [Adafruit TFT Shield](https://www.adafruit.com/product/1651) (1 per Arduino)
3. A server
4. A dev machine with `make`, `arduino-cli`, standard C++ build tools

## Setup

### Server
1. Clone this repo to your server
2. `$ make Server`
3. `$ ./bin/Server {server IP}`

Now a server will be open at `{server IP}:5341`. 

### Arduino
1. Clone this repo to your dev machine
2. Connect the Arduino to your dev machine
3. `$ ./scripts/configure.py`
4. `$ make Client`
5. `$ rm ./src/Client/secrets.h` (unless you want to expose your wifi password)
6. `$ ./scripts/upload.sh`

## Developer Notes
If you need any help setting this up, please contact me. I would love for someone to use this.