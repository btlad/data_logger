#!/usr/bin/env python

# Script was tested on the ArchLinux system where the python3 is used by default,
# therefore on some more conservative systems the shebang '#!/usr/bin/env python3' might be required

import serial
from pynput import keyboard
import time
from datetime import datetime
import os

data_storage_dir ="."
day_file = None
status = True
uart_cmd = b'1'

# configure the serial connection
ser = serial.Serial(
    port='/dev/ttyACM0',
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=10
)

# Short instruction
def print_usage():
    print('Simple data acquisition application example')
    print('Keyboard commands:')
    print('          1 - Set sample rate 1 sec')
    print('          2 - Set sample rate 2 sec')
    print('          3 - Set sample rate 3 sec')
    print('          4 - Set sample rate 4 sec')
    print('          5 - Set sample rate 5 sec')
    print('          6 - Set sample rate 6 sec')
    print('          7 - Set sample rate 7 sec')
    print('          8 - Set sample rate 8 sec')
    print('          9 - Set sample rate 9 sec')
    print('          s - Stop data acquisition')
    print('          r - Restore data acquisition')
    print('          q - Quit the program')
    print('          Ctrl + c raises the keyboard interrupt, which also causes') 
    print('                   program termination.\n')

# Header for file with results
def create_header():
    day_file.write('Day-Month-Year Hour:Min:Sec'+'      '+'P10[2] Voltage, V'+'\n')

# Output daily data file
def open_file():
    global day_file
    if os.path.isfile(data_storage_dir+"/"+datetime.now().strftime('%Y-%m-%d')):
        day_file = open(data_storage_dir+"/"+datetime.now().strftime('%Y-%m-%d'),'a')
        print('Daily file already exists, continue appending data to the file')
    else:
        day_file = open(data_storage_dir+"/"+datetime.now().strftime('%Y-%m-%d'),'a')
        create_header()
        print('Creating new daily file')

# Handling the keyboard
def on_press(key):
    global status
    global uart_cmd
    try:
        if key.char == '1':
            time.sleep(0.01)
            print(datetime.now().strftime("\n%H:%M:%S : Sampling rate 1 sec"))
            uart_cmd = b'1'
        elif key.char == '2':
            time.sleep(0.01)
            print(datetime.now().strftime("\n%H:%M:%S : Sampling rate 2 sec"))
            uart_cmd = b'2'
        elif key.char == '3':
            time.sleep(0.01)
            print(datetime.now().strftime("\n%H:%M:%S : Sampling rate 3 sec"))
            uart_cmd = b'3'
        elif key.char == '4':
            time.sleep(0.01)
            print(datetime.now().strftime("\n%H:%M:%S : Sampling rate 4 sec"))
            uart_cmd = b'4'
        elif key.char == '5':
            time.sleep(0.01)
            print(datetime.now().strftime("\n%H:%M:%S : Sampling rate 5 sec"))
            uart_cmd = b'5'
        elif key.char == '6':
            time.sleep(0.01)
            print(datetime.now().strftime("\n%H:%M:%S : Sampling rate 6 sec"))
            uart_cmd = b'6'
        elif key.char == '7':
            time.sleep(0.01)
            print(datetime.now().strftime("\n%H:%M:%S : Sampling rate 7 sec"))
            uart_cmd = b'7'
        elif key.char == '8':
            time.sleep(0.01)
            print(datetime.now().strftime("\n%H:%M:%S : Sampling rate 8 sec"))
            uart_cmd = b'8'
        elif key.char == '9':
            time.sleep(0.01)
            print(datetime.now().strftime("\n%H:%M:%S : Sampling rate 9 sec"))
            uart_cmd = b'9'
        elif key.char == 's':
            time.sleep(0.01)
            print(datetime.now().strftime("\n%H:%M:%S : Stop data acquisition"))
            uart_cmd = b's'
        elif key.char == 'r':
            time.sleep(0.01)
            print(datetime.now().strftime("\n%H:%M:%S : Restore data acquisition"))
            uart_cmd = b'r'
        elif key.char == 'q':
            time.sleep(0.01)
            print(datetime.now().strftime("\n%H:%M:%S : Quit, by."))
            status = False
        else:
            time.sleep(0.01)
            print(datetime.now().strftime("\n%H:%M:%S : Undefined command"))

    except AttributeError:
        None

def on_release(key):
    global uart_cmd
    ser.write(uart_cmd)

listener = keyboard.Listener(
    on_press=on_press,
    on_release=on_release)
listener.start()

# Start 
if ser.is_open:
    print_usage()
    ser.reset_output_buffer() # Clear output buffer, aborting the current output and discarding all that is in the buffer.
    ser.write(b's')           # Stop data flow from the controller board
    time.sleep(1)             # Give some time to flush the KitProg3 data buffer
    open_file()
    ser.reset_input_buffer()  # Flush input buffer, discarding all its contents
    ser.write(uart_cmd)       # Set initial sampling rate to 1 sec even if it is default for the board after the reset
    ser.write(b'r')           # Start ( {r}un or {r}estore ) the data acquisition
    ser.readline()            # Ignore first line which is sent immediately after ADC reactivating.
    print(datetime.now().strftime("%H:%M:%S : Start data acquisition"))

    try:
        while status:
            line = ser.readline()
            if line == b'':
                continue
            else:
                val = float(line)/1000.0
                day_file.write((datetime.now().strftime('%d-%m-%Y')+" "
                       +datetime.now().strftime("%H:%M:%S"))+"        "
                       +f"{val:6.3f}"+' V\n')

        raise KeyboardInterrupt

    except KeyboardInterrupt:
        ser.close()
        day_file.close()
        print('\nDaily file closed')
#
else:
    print('cannot open serial port ')
