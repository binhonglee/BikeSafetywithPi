# Bike Safety with Pi

A raspberry pi to be mounted on a bike to alert surrounding and the emergency contact of the rider if they fall.

## Hardware requirements

| Required | What I used |
|------|-----|
| Raspberry Pi | Pi 3 Model B |
| MPU3050 gyroscope | OSEPP GYROS-01 |
| Button interrupt | OSEPP PUSH-01 |
| Buzzer |

## Code dependencies

1. g++
2. ssmtp
3. [wiringPi](http://wiringpi.com/download-and-install/)

## Setup

1. If you don't have ssmtp installed, run this
  ```sh
  $ sudo apt-get update && apt-get install ssmtp
  ```
2. Run the setup script
  ```sh
  $ ./setup.sh
  ```
3. Input the email you want to sent to
4. Done!

## Usage

```
$ ./compileandexecute.sh
```
