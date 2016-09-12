Required Hardware:

A functional USB Audio Interface is ideal to use with the software, such as the Behringer UCA202 in order to ensure minimal latency during real-time audio processing. 

Software Directions:

1) Install Pure-Data Extended on Raspbian via the following Steps:

Run these commands from the terminal: 

$ sudo nano /etc/apt/sources.list 

-copy/paste this line to the list(right-click to paste): 
deb-src http://archive.raspbian.org/raspbian wheezy main contrib non-free rpi 

ctrl+o, Enter (to save). ctrl+x (to exit) 


$ sudo apt-get update 

$ wget https://puredata.info/downloads/pd-extended-0-43-3-on-raspberry-pi-raspbian-wheezy-armhf/releases/1.0/Pd-0.43.3-extended-20121004.deb 

$ sudo dpkg -i Pd-0.43.3-extended-20121004.deb 

$ sudo apt-get -f install 

it should be installed. For root priorities: 

$ sudo chmod 4755 /usr/bin/pd-extended 

2) In the mmb folder, run MainPatch.pd on Pd-extended

3) Compile and run guitar_effects.c using the following command on terminal:

$ gcc -o ./guitar_effects guitar_effects.c -lwiringPi
$ sudo ./guitar_effects 0
