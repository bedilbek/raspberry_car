import argparse
import socket

import pygame
import time


pygame.init()
j = pygame.joystick.Joystick(0)
j.init()
print 'Detected controller : %s' % j.get_name()

ap = argparse.ArgumentParser()
ap.add_argument("-t", "--throttle", type=int, required=False, default=1,
                help="Set throttle axis")
ap.add_argument("-s", "--steering", type=int, required=False, default=3,
                help="Set steering axis")
ap.add_argument("-q", "--step", type=int, required=False, default=5,
                help="Set step for change of steering and throttle")
ap.add_argument("-p", "--port", type=int, required=False, default=8059,
                help="Set port to connect as a client to socket")
ap.add_argument("-g", "--host", type=str, required=False, default="localhost",
                help="Set host to connect as a client to socket")
ap.add_argument("-f", "--frequency", type=int, required=False, default=20,
                help="How many times a second to check for new values")
ap.add_argument("-b", "--break_button", type=int, required=False, default=7,
                help="Set break out button to exit")
ap.add_argument("--debug", dest='debug', action='store_true',
                help="Debug options")
ap.add_argument("--socket", dest='socket', action='store_true', default=False,
                help="Enable socket")

args = vars(ap.parse_args())

AXIS_ANALOG1_X = 0
AXIS_ANALOG1_Y = 1
AXIS_L2 = 2
AXIS_ANALOG2_X = 3
AXIS_ANALOG2_Y = 4
AXIS_R2 = 5

BUTTON_X = 0
BUTTON_ROUND = 1
BUTTON_TRIANGLE = 2
BUTTON_SQUARE = 3
BUTTON_L1 = 4
BUTTON_R1 = 5
BUTTON_L2 = 6
BUTTON_R2 = 7
BUTTON_SELECT = 8
BUTTON_START = 9
BUTTON_PS = 10
BUTTON_ANALOG1 = 11
BUTTON_ANALOG2 = 12

# Select which axes to check
THROTTLE_AXIS = args['throttle']
STEERING_AXIS = args['steering']
FREQUENCY_CHECK = args['frequency']
BREAK_BUTTON = args['break_button']
STEP = args['step']
axes_to_check = [AXIS_ANALOG1_Y, AXIS_ANALOG2_X]
latest_throttle_value = 0
latest_throttle__axis_value = 0
latest_steering_value = 0
started = False
# Begin reading the controller

if args['debug']:
    print 'DEBUGGING ENABLED'
    print 'THROTTLE AXIS is set to %i' % THROTTLE_AXIS
    print 'STEERING AXIS is set to %i' % STEERING_AXIS
    print 'FREQUENCY CHECK is set to %i' % FREQUENCY_CHECK
    print 'BREAK BUTTON is set to %i' % BREAK_BUTTON
    print 'STEP is set to %i' % STEP
    if args['socket']:
        print 'SOCKET ENABLED'

if args['socket']:
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_address = (args['host'], args['port'])
    sock.connect(server_address)

try:
    while True:
        pygame.event.pump()
        changed = False
        for current_axis in axes_to_check:
            new_value = j.get_axis(current_axis)
            new_value *= (100 / 1.4)
            if current_axis == STEERING_AXIS and (abs(new_value) > (abs(latest_steering_value) + STEP) or abs(new_value) <
                                                  (abs(latest_steering_value) - 5)):
                latest_steering_value = new_value
                changed = True

        if j.get_button(BUTTON_L2) == 1:
            changed = True
            if latest_throttle_value == -70:
                changed = False
            latest_throttle_value = -70

        if j.get_button(BUTTON_L1) == 1:
            changed = True
            if latest_throttle_value == -90:
                changed = False
            latest_throttle_value = -90

        if j.get_button(BUTTON_R2) == 1:
            changed = True
            if latest_throttle_value == -60:
                changed = False
            latest_throttle_value = -60

        if j.get_button(BUTTON_R1) == 1:
            changed = True
            if latest_throttle_value == -80:
                changed = False
            latest_throttle_value = -80

        if j.get_button(BUTTON_ROUND) == 1:
            changed = True
            if latest_throttle_value == 0:
                changed = False
            latest_throttle_value = 0

        if j.get_button(BUTTON_X) == 1:
            changed = True
            if started:
                changed = False
            started = True

        if j.get_button(BUTTON_SQUARE) == 1:
            changed = True
            if not started:
                changed = False
            started = False

        if changed:
            if args['debug']:
                print 'throttle: %.2f steering: %.2f' % (latest_throttle_value, latest_steering_value)
            message = ""
            throttle_direction = 1 if latest_throttle_value < 0 else 0
            message = message + chr(throttle_direction)
            message = message + chr(abs(int(latest_throttle_value)))
            steering_direction = 0 if latest_steering_value < 0 else 1
            message = message + chr(steering_direction)
            message = message + chr(abs(int(latest_steering_value)))
            message = message + chr(int(started))
            print started
            if args['socket']:
                sent = sock.sendto(bytes(message), server_address)

        time.sleep(1 / FREQUENCY_CHECK)

        # if j.get_button(BREAK_BUTTON) == 1:
        #     break
finally:
    if args['socket']:
        print('closing socket')
        sock.close()


