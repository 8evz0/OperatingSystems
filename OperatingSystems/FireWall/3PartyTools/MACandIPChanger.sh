#!/bin/bash

C_O='\033[0m'
GRN='\033[0;32m'


while (true)
do
	x=1
	while [ $x -le 30 ]
	do
		echo -e "${GRN} ---> NEW MAC ${C_O}"
		sudo macchanger -r enp0s3
		 ifconfig
		#sudo /etc/init.d/networking restart
		sleep 10
		echo -e "${GRN} ---> NEW MAC ${C_O}"
		sudo macchanger -r enp0s3
		ifconfig
		#sudo /etc/init.d/networking restart	
		sleep 15
		x=$(( $x + 1 ))
	done
	echo -e "${GRN} ---> NEW IP ${C_O}"
	#sudo ./IPChanger enp3s0 $(( $RANDOM % 255 + 1 )).$(( $RANDOM % 255 + 1 )).$(( $RANDOM % 255 + 1)).$(( $RANDOM % 255 + 1))
	##sudo ifconfig enp3s0 down
	#sudo ifconfig enp3s0 up
	#systemctl restart network
	echo -e "${GRN} <--- END ${C_O} \n"
	sudo macchanger -r lo
done








