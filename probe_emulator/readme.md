In this folder, one can find tool for compiling a program that will emul an echopen probe. This emulator can run on a PC or on a RedPitaya (to be close to a real kit).

For compiling the C code (./srcbin/probe_emulator.c), use the bash file run.sh provide here. Option for choosing the device on which the program will run use -m option, with option = RP for RedPitaya and option = PC for your PC such as:

	sh run.sh -m RP

If you chose PC option, the executable file will be located in this folder. If you chose RP option, the executable will be send in the RedPitaya. This executable need an option in: void, plate, hand and film:

	./probe_emulator plate

* void option will send a loop with an image fill with only value 8192,
* plate option will send a loop with an unique image of a plate,
* hand option will send a loop with an unique image of the cross-section of a hand,
* film will send a loop of an echographic film of the cross-sections of a hand an harm.

To execute the program on the RedPitaya you first need to connect to the RedPitaya *via* ssh:

	ssh root@192.168.128.3

pass is root. At connection you will be situated in /root folder where is located the program. Now you can just lanch it (with correct option).

