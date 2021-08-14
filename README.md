# PSoC 6 MCU: Data Logger

This code implements a simple data acquisition system for sampling slowly changing analog values and writes them to an ASCII file.
It can be used as a basis for creating a weather station, ie to record temperature, atmospheric pressure, humidity, etc.

The project consists of two parts:

- the first part is implemented as a ModusToolbox project for the CY8CPROTO-062S3-4343W prototype board;
- the second one is the Python script (pc-code / GrabControlWrite.py), which intends to run on a desktop PC or minicomputer (for example, Raspberry Pi).

 **Microcontroller code**

Microcontroller part of the project is derived from the [ADC_basic](https://github.com/Infineon/mtb-example-psoc6-adc-basic) application example.

It performs:

- periodic sampling of analog voltage at input pin P10[2] (see note below);
- sends the measured values to the UART port;
- stop and resume the data collection process, as well as change the sampling rate according to the commands coming from the UART.
In general, the microcontroller receives eleven single-byte commands: 's' -stop,' r'- restore, from '1' to '9' - sampling rate in seconds;

 '#define UNTITLED'

If the UNTITLED macro is not defined before the project is built and its code is programmed into the microcontroller,
the text header and additional information about the measured values and sampling rate will be present on the UART port,
which is useful for debugging reasons.
The terminal emulator program, ie
[GNU screen](https://www.gnu.org/software/screen/),
[Cool Term](http://freeware.the-meiers.org/),
[Tera Term](https://ttssh2.osdn.jp/index.html.en), etc., are sutable in this case. (The "GNU screen" was used for this project.)

If the UNTITLED macro is defined, the microcontroller sends to the UART only the digitized values of the measured voltage (in millivolts),
one line per sample with a certain sampling rate.

**PC code**

PC code is a Python script (pc-code/GrabControlWrite.py) that works exactly as specified in its file name.
It :

- grabs the data from the UART (see note below);
- have a ability to pass several commands to the microcontroller;
- records the received data into the output file.

After running the program it:

- displays brief information about the keyboard commands it is able to receives and executes;
- connects to a defined serial port;
- pays attention to the release of all buffers (even in the USB-UART converter which is unreachable by code);
- starts collecting data with a sampling rate of 1 s, regardless of the state in which the microcontroller was previously.

The data are recorded to daily files, so the script checks, if the daily file has been created before.
If not, the script creates a new file, otherwise it opens the existing file to append data.
Each file contains a header that describes the data columns 

'Day-Month-Year Hour:Min:Sec      P10[2] Voltage, V'

and the data themselves are recorded with timestamps.
No additional information is recorded into the data file when the sampling rate changes or data collection is stopped and resumed.
In total, the script accept 13 commands from the keyboard:

- '1' - Set sample rate 1 sec;
- '2' - Set sample rate 2 sec;
- '3' - Set sample rate 3 sec;
- '4' - Set sample rate 4 sec;
- '5' - Set sample rate 5 sec;
- '6' - Set sample rate 6 sec;
- '7' - Set sample rate 7 sec;
- '8' - Set sample rate 8 sec;
- '9' - Set sample rate 9 sec;
- 's' - Stop data acquisition;
- 'r' - Restore data acquisition;
- 'q' - Quit the program;
- 'Ctrl + c' - raises the keyboard interrupt, which is handled and causes the program termination also.

**Note:** The pins P10[0] and P10[1] are hard-wired on the 
PSoC 62S3 Wi-Fi BT Prototyping Kit (CY8CPROTO-062S3-4343W) board.

see:
- 2.2.2 USB-UART Bridge,
- 3.2.3 Serial Interconnection between PSoC 5LP and PSoC 6 MCU,
- 3.2.3.1 BT UART

in the "PSoC 62S3 Wi-Fi BT Prototyping Kit Guide". Therefore these pins cannot be easily used for an ADC purposes.

The **easist** way to run ADC_basic Appliucation Example as well as projects
derived from it on the PSoC 62S3 Wi-Fi BT Prototyping Kit (CY8CPROTO-062S3-4343W) board is to use P10[2] and P10[3] instead.

**Note:** The Python script was developed and tested on the PC powered by Arch Linux.
On the Windows the serial port have to be changed to something like 'COM*' according to information from the "Windows Device Manager".
