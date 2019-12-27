#!/usr/bin/env python2.7
from smbus2 import SMBusWrapper
import time
import rospy
import signal
from telemetry.msg import led
ADDRESS = 0x04

roverMode = 0
frequency = 0


def sigint_handler(signum, frame):
    print("CTRL+C Pressed!")
    exit()

signal.signal(signal.SIGINT, sigint_handler)

def writeToBus(roverMode, frequency):
    #if roverMode not in range(11) or frequency not in range(11):
    #return False
    #if type(mode) != int or type(freq) != int:
    #return False
    modeTemp = 10
    freqTemp = 10
    if roverMode != modeTemp and frequency != freqTemp:
         with SMBusWrapper(1) as bus:
            bus.write_byte_data(ADDRESS, roverMode, frequency)
            modeTemp = roverMode
            freqTemp = frequency
    else:
        pass

def callback(data):
    roverMode = data.mode
    frequency = data.freq
    writeToBus(roverMode, frequency)

def led_sub():
    rospy.init_node('ledSubscriber')
    rospy.Subscriber("/led", led, callback)
    rospy.spin()


def main():
    led_sub()

main()
#    if __name__ == '__main__':
#        main()