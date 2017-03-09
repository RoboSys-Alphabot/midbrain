#!/usr/bin/env python

import rospy
from smach import *
from smach_ros import *


# define state Foo
class Foo(State):
    def __init__(self):
        State.__init__(self, outcomes=['outcome1', 'outcome2'])
        self.counter = 0

    def execute(self, userdata):
        rospy.loginfo('Executing state FOO')
        if self.counter < 3:
            self.counter += 1
            return 'outcome1'
        else:
            return 'outcome2'


# define state Bar
class Bar(State):
    def __init__(self):
        State.__init__(self, outcomes=['outcome2'])

    def execute(self, userdata):
        rospy.loginfo('Executing state BAR')
        return 'outcome2'


# main
def main():
    rospy.init_node('smach_example_state_machine')

    # Create a SMACH state machine
    sm = StateMachine(outcomes=['outcome4', 'outcome5'])

    # Open the container
    with sm:
        # Add states to the container
        StateMachine.add('FOO', Foo(),
                         transitions={'outcome1': 'BAR',
                                      'outcome2': 'outcome4'})
        StateMachine.add('BAR', Bar(),
                         transitions={'outcome2': 'FOO'})

    # Execute SMACH plan
    outcome = sm.execute()


if __name__ == '__main__':
    main()
