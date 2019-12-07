#!/usr/bin/env python
'''
XBOX Controller
axis 0: LR - left stick -/+
axis 1: UD - left stick -/+
axis 2: LR - right stick -/+
axis 3: UD - right stick -/+
axis 4: default = neg, pressed = pos - top right
axis 5: default = neg, pressed = pos - top left
axis 6: LR - cross -/+
axis 7: UP - cross -/+
B0: A
B1: B
B3: X
B4: Y
B6: top left push
B7: top right push
B11: mid-right
'''
import rospy
import socket
import struct
import threading
from time import sleep
import message_filters as mf
from multijoy.msg import Multi
from sensor_msgs.msg import Joy

class MultiJoyParser(object):

    def __init__(self):

        # Retrieve parameters
        self.ns=rospy.get_name()
        self.param_name_debug=self.ns+'/debug'
        self.param_name_njoys=self.ns+'/njoys'

        if rospy.has_param(self.param_name_debug):
            self.debug=rospy.get_param(self.param_name_debug)
        else:
            self.debug=False
        #print rospy.get_param(self.param_name_njoys)
        self.njoys=rospy.get_param(self.param_name_njoys)

        if self.debug:
            rospy.loginfo('debug={}'.format(self.debug))
            rospy.loginfo('njoys={}'.format(self.njoys))

        # Setup ros publisher
        self.multijoy_pub=rospy.Publisher('/multijoy', Multi, queue_size=1)

        # Setup ros subscribers
        self.joy_subs=[mf.Subscriber('/joy/'+str(i),Joy, queue_size=1) for i in xrange(self.njoys)]
        self.timeSync=mf.ApproximateTimeSynchronizer(self.joy_subs, 10, self.njoys*100)
        self.timeSync.registerCallback(self.update)

    def update(self, *args):
        msg=Multi()
        msg.header.stamp=rospy.Time.now()
        msg.njoys.data=self.njoys
        msg.joys=args
        msg.source=0
        self.multijoy_pub.publish(msg)

if __name__=='__main__':
    rospy.init_node('multijoy_node')
    parser=MultiJoyParser()
    rospy.spin()

