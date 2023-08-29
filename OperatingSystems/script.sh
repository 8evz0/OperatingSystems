#!/bin/bash

os=$(grep -oP '(?<=^ID=).+' /etc/os-release | tr -d '"')
work_dir=$PWD

# Reset
Color_Off='\033[0m'       # Text Reset

# Regular Colors
Red='\033[0;31m'          # Red
Green='\033[0;32m'        # Green
Yellow='\033[0;33m'       # Yellow
Purple='\033[0;35m'       # Purple
Cyan='\033[0;36m'         # Cyan

# Bold
BRed='\033[1;31m'         # Red
BGreen='\033[1;32m'       # Green
BYellow='\033[1;33m'      # Yellow
BPurple='\033[1;35m'      # Purple
BCyan='\033[1;36m'        # Cyan

InfoBlock()
{
	echo -e "No help" |  boxes -d parchment -p a4
	echo -e "${BCyan}" 
	read -p "Press enter to go back..."
	echo -e "${Color_Off}"
}

AuthorContacts()
{
	echo -e "Created by: C. Leo\t\nE-mail: dom_bevzo@mail.ru\t\nTelegram: @l_evv\n\t2023" |  boxes -d santa -p a4
	echo -e "${BCyan}" 
	read -p "Press enter to go back..."
	echo -e "${Color_Off}"
}

install_programs() 
{
	case $os in
		debian|ubuntu)
		packages=(
			["hdparm"]="hdparm"
			["bonnie++"]="bonnie++"
			["sysstat"]="sysstat"
			["fio"]="fio"
			["lolcat"]="lolcat"
			["figlet"]="figlet"
			["xcowsay"]="xcowsay"
			["cowsay"]="cowsay"
			["fortune"]="fortune"
			["boxes"]="boxes"
			["tmux"]="tmux"
			["libpam0g-dev"]="libpam0g-dev"
			["libnfnetlink"]="libnfnetlink"
			["libnfnetlink-dev"]="libnfnetlink-dev"
			["libnetfilter-queue-dev"]="libnetfilter-queue-dev"
			["iptables"]="iptables"
			["pam-utils"]="pam-utils"
			["pam"]="pam"
			["nasm"]="nasm"
			["base-devel"]="base-devel"
			["base-devel"]="net-tools"
			
		)
		for package in "${!packages[@]}"; do
			if ! dpkg -s "${packages[$package]}" > /dev/null 2>&1; then
				sudo apt-get install -y "${packages[$package]}"
				
			else
				echo "$package is already installed."
			fi
		done
		sudo apt install boxes
		;;
		
		arch)
		declare -A packages=(
			["hdparm"]="hdparm"
			["bonnie++"]="bonnie++"
			["sysstat"]="sysstat"
			["fio"]="fio"
			["lolcat"]="lolcat"
			["figlet"]="figlet"
			["xcowsay"]="xcowsay"
			["cowsay"]="cowsay"
			["fortune"]="fortune"
			["cowfortune"]="cowfortune"
			["boxes"]="boxes"
			["tmux"]="tmux"
			["libpam0g-dev"]="libpam0g-dev"
			["libnfnetlink"]="libnfnetlink"
			["libnfnetlink-dev"]="libnfnetlink-dev"
			["libnetfilter-queue-dev"]="libnetfilter-queue-dev"
			["iptables"]="iptables"
			["nasm"]="nasm"
			["pam-utils"]="pam-utils"
			["pam"]="pam"
			["base-devel"]="base-devel"
			["inetutils"]="inetutils"
		)
		for package in "${!packages[@]}"; do
			if ! pacman -Qs "${packages[$package]}" > /dev/null 2>&1; then
				sudo pacman -S "${packages[$package]}"
			else
				echo "$package is already installed."
			fi
		done
		
		if ! pacman -Qs yaourt > /dev/null 2>&1; then
			current_user=$(logname)
			sudo pacman -S --needed base-devel git wget yajl
			cd /tmp
			git clone https://aur.archlinux.org/package-query.git
			cd package-query/
			makepkg -si && cd /tmp/
			git clone https://aur.archlinux.org/yaourt.git
			cd yaourt/
			
			makepkg -si

		else
			echo "yaourt is already installed"
		fi
		
		for package in "${!packages[@]}"; do
			if ! yaourt -Q "${packages[$package]}" > /dev/null 2>&1; then
				yaourt -S "${packages[$package]}"
			else
				echo "$package is already installed."
			fi
		done
		
		sleep 20
		;;
		*)
		echo "The package manager for this operating system could not be detected."
		exit 1
		;;
	esac
}

remove_programs() 
{
	case $os in
		debian|ubuntu)
		packages=(
			["hdparm"]="hdparm"
			["bonnie++"]="bonnie++"
			["sysstat"]="sysstat"
			["fio"]="fio"
			["lolcat"]="lolcat"
			["figlet"]="figlet"
			["xcowsay"]="xcowsay"
			["cowsay"]="cowsay"
			["fortune"]="fortune"
			["boxes"]="boxes"
			["tmux"]="tmux"
			["nasm"]="nasm"
			["libpam0g-dev"]="libpam0g-dev"
			["libnfnetlink"]="libnfnetlink"
			["libnfnetlink-dev"]="libnfnetlink-dev"
			["libnetfilter-queue-dev"]="libnetfilter-queue-dev"
			["iptables"]="iptables"
			["pam-utils"]="pam-utils"
			["pam"]="pam"
			["base-devel"]="base-devel"
		)
		for package in "${!packages[@]}"; do
			sudo apt-get delete -y "${packages[$package]}"
		done
		;;
		arch)
		declare -A packages=(
			["hdparm"]="hdparm"
			["bonnie++"]="bonnie++"
			["sysstat"]="sysstat"
			["fio"]="fio"
			["lolcat"]="lolcat"
			["figlet"]="figlet"
			["xcowsay"]="xcowsay"
			["cowsay"]="cowsay"
			["fortune"]="fortune"
			["boxes"]="boxes"
			["tmux"]="tmux"
			["nasm"]="nasm"
			["libpam0g-dev"]="libpam0g-dev"
			["libnfnetlink"]="libnfnetlink"
			["libnfnetlink-dev"]="libnfnetlink-dev"
			["libnetfilter-queue-dev"]="libnetfilter-queue-dev"
			["iptables"]="iptables"
			["pam-utils"]="pam-utils"
			["pam"]="pam"
			["base-devel"]="base-devel"
		)
		for package in "${!packages[@]}"; do
			sudo pacman -R "${packages[$package]}"
		done
		
		for package in "${!packages[@]}"; do
				yaourt -R "${packages[$package]}"
		done
		
		sudo pacman -R --needed base-devel git wget yajl
		sudo rm -rf /tmp/package-query
		sudo pacman -R yaourt
		current_user=$(logname)
		sudo -u "$current_user" yaourt -R boxes
		;;
		*)
		echo "The package manager for this operating system could not be detected."
		exit 1
		;;
	esac
	main
}


Linpack()
{
	echo "Linpack tests" |boxes -d tux -p a1 
	
	cd LinPack
	sudo git clone https://github.com/ereyes01/linpack.git 
	cd linpack && sudo make 
	
	restore_default() 
	{
	  local param_name="$1"
	  local default_value="$2"

	  echo "$default_value" > "/proc/sys/kernel/sched_$param_name"
	}

	cd /proc/sys/kernel/

	echo -e "${BPurple}List of parameters in your OS:${Color_Off}"
	for file in sched_*; do
	  param_name="${file#sched_}"
	  current_value=$(cat "$file")
	  echo -e "${Yellow}sched_$param_name: $current_value ${Color_Off}"
	done

	param_list=$(ls sched_* | sed 's/sched_//')

	echo -e "${Green}Enter the name of the parameter to change (or 'q' to quit) enter whithout 'sched_':${Color_Off}"
	read -p "" chosen_param

	while [[ "$chosen_param" != "q" ]]; 
	do
	  if [[ "$param_list" =~ (^|[[:space:]])"$chosen_param"($|[[:space:]]) ]]; then
	    current_value=$(cat "sched_$chosen_param")

	    echo -e "${Green}Current value '$chosen_param': $current_value. Enter the new value:  ${Color_Off}" 
	    read -p "" new_value

	    echo "$new_value" > "sched_$chosen_param"

	    echo -e "${Green}Parameter value '$chosen_param' successfully changed to '$new_value'.${Color_Off}"
	  else
	    echo -e "${Red}Wrong parameter name. Try again.${Color_Off}"
	  fi

	  echo -e "${Green}Enter the name of the parameter to change (or 'q' to quit): ${Color_Off}" 
	  read -p "" chosen_param
	done

	echo -e "${Green}Do you want to restore the default values? (y/n): ${Color_Off}"
	read -p "" restore_default_choice

	if [[ "$restore_default_choice" == "y" ]]; then
	  for param in $param_list; do
	    default_value=$(cat "sched_$param")
	    restore_default "$param" "$default_value"
	    echo -e "${Yellow}Parameter value '$param' restored by default: $default_value ${Color_Off}"
	  done
	fi
	
	cd $BASEDIR
	cd linpack
	echo "Routine testing" | lolcat 
	for ((i=1; i<=2; i++))
	do
		./linpack 
	done
	
	echo "Testing the program with the lowest task priority" | lolcat 
	for ((i=1; i<=2; i++))
	do
		nice -n -19 ./linpack 
	done
	
	echo "Testing a program with the highest task priority" | lolcat 
	for ((i=1; i<=2; i++))
	do
		nice -n 20 ./linpack
	done
	
	echo "Testing a program with processor binding" | lolcat
	for ((i=1; i<=2; i++))
	do
		taskset -c 0 ./linpack
		taskset -c 1,2 ./linpack 
	done
	
	echo "Testing a program without processor binding" | lolcat
	for ((i=1; i<=2; i++))
	do
		./linpack 
	done

	cd $BASEDIR
	
	read -p "Tests is over. Check results in './logs'. Press Enter to delete all test's files and clear console:"
	rm -rf $BASEDIR/linpack
	
	cd $workdir
}

## CHOOSE BLOCK 
BLOCK()
{
	echo -e "${Green}Choose disk which has I/O Schedulers${Color_Off}"
	lsblk -l;
	read -p "Enter disk name (sda/sdb/..): " BLOCK_
	schedulers=/sys/block/$BLOCK_/queue/scheduler
	clear
}

##

## INFO BLOCK
I_B()
{
	echo -e "${BGreen}____System information____${Color_Off}"
	hostnamectl
}
##

## AVAILABLE SCHEDULERS
A_S()
{
	echo -e "${BGreen}____Available I/O Schedulers____${Color_Off}"
	echo -e "${Yellow}"[name]" - default scheduler${Color_Off}"
	awk -v OFS="" -v FPAT='\\[[^]]*\\]|"[^"]*"|[^[:space:]]+' '{
		for (i=1; i<=NF; i++) 
		{
			gsub(/^[]|[]$/,$i)
			$i =  i ".\""$i"\"\n"
		}
	} 1' $schedulers
}
A_S_After_Change()
{
	echo -e "${BCyan}Defualt scheduler was changed!${Color_Off}"
}
##

## TEST SCHEDULER
T_S()
{
	echo "I/O Scheduler tests" |boxes -d tux -p a1 
	wget https://www.kernel.org/pub/linux/kernel/v3.x/linux-3.13.7.tar.xz

	
	for T in none mq-deadline kyber bfq;
	do

		sudo rm -rf $BASEDIR/linux-3.13.7
		sudo rm $BASEDIR/benchfile
	
		echo 3 > /proc/sys/vm/drop_caches
		
		echo -e "${BYellow}SCHEDULER\t--->\t"$T"${Color_Off}"
		echo -e "${BPurple}IOSTAT_INFORMATION${Color_Off}"
		
		iostat -p
		
		echo -e "${BPurple}TEST_BONNIE${Color_Off}"
		mkdir boniee_test_dir
		bonnie++ -u root -d $BASEDIR/boniee_test_dir
		rm -rf boniee_test_dir
		echo 3 > /proc/sys/vm/drop_caches
		
		echo -e "${BPurple}TEST_FIO.Test random read/write${Color_Off}"
		sudo fio --filename=$BASEDIR/path --size=1GB --direct=1 --rw=randrw --bs=4k --ioengine=libaio --iodepth=256 --runtime=10 --numjobs=4 --time_based --group_reporting --name=job_name --eta-newline=1
		echo 3 > /proc/sys/vm/drop_caches
		
		echo -e "${BPurple}TEST_HDPARM${Color_Off}"
		echo $T > /sys/block/$BLOCK_/queue/scheduler; 
		cat /sys/block/$BLOCK_/queue/scheduler; 
		sync && /sbin/hdparm -tT /dev/$BLOCK_ && echo "----"; 
		echo 3 > /proc/sys/vm/drop_caches
		
		echo -e "${BPurple}TEST_DD${Color_Off}"
		for i in 1 2 3 4 5; 
		do
			time dd if=$BASEDIR/path of=./benchfile bs=1M count=19900 conv=fdatasync,notrunc
			echo 3 > /proc/sys/vm/drop_caches
		done
		echo 3 > /proc/sys/vm/drop_caches
		
		echo -e "${BPurple}TEST_TAR${Color_Off}"
		for i in 1 2 3 4 5; 
		do
			time tar xJf linux-3.13.7.tar.xz
			echo 3 > /proc/sys/vm/drop_caches
		done
		
	done
	#yaourt -R sysstat fio hdparm
	rm $BASEDIR/path
	rm -rf $BASEDIR/linux-3.13.7
	rm $BASEDIR/benchfile
	rm $BASEDIR/linux-3.13.7.tar.xz
}
##

## CHANGE PARAMETRS IF MQ-DEADLINE SCHEDULER
C_P_Of_mq_deadline()
{
	echo -e "${BPurple}You can change these parameters of mq-deadline\n${Color_Off}${Green}\t1.async_depth \n\t2.front_merges \n\t3.read_expire\n\t4.writes_starved\n\t5.fifo_batch\n\t6.prio_aging_expire\n\t7.write_expire\n\t8. Back to the start menu\n\t9. Reset to default${Color_Off}"
	read -p "Enter 1-9: " S_To_Change
	case $S_To_Change in
		1) echo "async_depth" && cat /sys/block/sda/queue/iosched/async_depth && read -p "Enter new value: " async_depth && echo $async_depth > /sys/block/sda/queue/iosched/async_depth && C_P_Of_mq_deadline ;;
		2) echo "front_merges" && cat /sys/block/sda/queue/iosched/front_merges && read -p "Enter new value: " front_merges && echo $front_merges > /sys/block/sda/queue/iosched/front_merges && C_P_Of_mq_deadline;;
		3) echo "read_expire" && cat /sys/block/sda/queue/iosched/read_expire && read -p "Enter new value: " read_expire && echo $read_expire > /sys/block/sda/queue/iosched/read_expire && C_P_Of_mq_deadline;;
		4) echo "writes_starved" && cat /sys/block/sda/queue/iosched/writes_starved && read -p "Enter new value: " writes_starved && echo $writes_starved > /sys/block/sda/queue/iosched/writes_starved && C_P_Of_mq_deadline;;
		5) echo "fifo_batch" && cat /sys/block/sda/queue/iosched/fifo_batch && read -p "Enter new value: " fifo_batch && echo $fifo_batch > /sys/block/sda/queue/iosched/fifo_batch && C_P_Of_mq_deadline;;
		6) echo "prio_aging_expire" && cat /sys/block/sda/queue/iosched/prio_aging_expire && read -p "Enter new value: " prio_aging_expire && echo $prio_aging_expire > /sys/block/sda/queue/iosched/prio_aging_expire && C_P_Of_mq_deadline;;
		7) echo "write_expire" && cat /sys/block/sda/queue/iosched/write_expire && read -p "Enter new value: " write_expire&& echo $write_expire > /sys/block/sda/queue/iosched/write_expire && C_P_Of_mq_deadline;;
		8) echo 48 > /sys/block/sda/queue/iosched/async_depth && echo 1 > /sys/block/sda/queue/iosched/front_merges && echo 500 > /sys/block/sda/queue/iosched/read_expire && echo 2 > /sys/block/sda/queue/iosched/writes_starved && echo 16 > /sys/block/sda/queue/iosched/fifo_batch && echo 10000 > /sys/block/sda/queue/iosched/prio_aging_expire && echo 5000 > /sys/block/sda/queue/iosched/write_expire;;
		9) main;;
	esac
}
##
C_P_Of_kyber()
{
	echo -e "${BPurple}You can change these parameters of kyber\n${Color_Off}${Green}\t1.read_lat_nsec \n\t2.write_lat_nsec \n\t3. Back to the start menu\n\t4. Reset to default${Color_Off}"
	read -p "Enter 1-2: " S_To_Change
	case $S_To_Change in
		1) echo "read_lat_nsec" && cat /sys/block/$BLOCK_/queue/iosched/read_lat_nsec && read -p "Enter new value: " read_lat_nsec && echo $read_lat_nsec > /sys/block/$BLOCK_/queue/iosched/read_lat_nsec && C_P_Of_kyber ;;
		2) echo "write_lat_nsec" && cat /sys/block/$BLOCK_/queue/iosched/write_lat_nsec && read -p "Enter new value: " write_lat_nsec && echo $write_lat_nsec > /sys/block/$BLOCK_/queue/iosched/write_lat_nsec && C_P_Of_kyber;;
		3) echo 2000000 > /sys/block/$BLOCK_/queue/iosched/read_lat_nsec && echo 10000000 > /sys/block/$BLOCK_/queue/iosched/write_lat_nsec;;
		4) main;;
	esac
	
}
##

## CHANGE PARAMETRS IF BFQ SCHEDULER
C_P_Of_bfq()
{
	#/sys/block/$BLOCK_/queue/iosched
	echo -e "${BPurple}You can change these parameters of BFQ\n${Color_Off}${Green}\t1.back_seek_max\n\t2.fifo_expire_sync\n\t3.slice_idle\n\t4.timeout_sync\n\t5.back_seek_penalty\n\t6.low_latency\n\t7.slice_idle_us\n\t8.fifo_expire_async\n\t9.max_budget\n\t10.strict_guarantees\n\t11. Back to the start menu\n\t12. Reset to default${Color_Off}"
	read -p "Enter 1-12: " S_To_Change
	case $S_To_Change in
		1) echo "back_seek_max" && cat /sys/block/$BLOCK_/queue/iosched/back_seek_max && read -p "Enter new value: " back_seek_max && echo $back_seek_max > /sys/block/$BLOCK_/queue/iosched/back_seek_max && C_P_Of_bfq;;
		2) echo "fifo_expire_sync" && cat /sys/block/$BLOCK_/queue/iosched/fifo_expire_sync && read -p "Enter new value: " fifo_expire_sync && echo $fifo_expire_sync > /sys/block/$BLOCK_/queue/iosched/fifo_expire_sync && C_P_Of_bfq;;
		3) echo "slice_idle" && cat /sys/block/$BLOCK_/queue/iosched/slice_idle && read -p "Enter new value: " slice_idle && echo $slice_idle > /sys/block/$BLOCK_/queue/iosched/slice_idle && C_P_Of_bfq;;
		4) echo "timeout_sync" && cat /sys/block/$BLOCK_/queue/iosched/timeout_sync && read -p "Enter new value: " timeout_sync && echo $timeout_sync > /sys/block/$BLOCK_/queue/iosched/timeout_sync && C_P_Of_bfq;;
		5) echo "back_seek_penalty" && cat /sys/block/$BLOCK_/queue/iosched/back_seek_penalty && read -p "Enter new value: " back_seek_penalty && echo $back_seek_penalty > /sys/block/$BLOCK_/queue/iosched/back_seek_penalty && C_P_Of_bfq;;
		6) echo "low_latency" && cat /sys/block/$BLOCK_/queue/iosched/low_latency && read -p "Enter new value: " low_latency && echo $low_latency > /sys/block/$BLOCK_/queue/iosched/low_latency && C_P_Of_bfq;;
		7) echo "slice_idle_us" && cat /sys/block/$BLOCK_/queue/iosched/slice_idle_us && read -p "Enter new value: " slice_idle_us && echo $slice_idle_us > /sys/block/$BLOCK_/queue/iosched/slice_idle_us && C_P_Of_bfq;;
		8) echo "fifo_expire_async" && cat /sys/block/$BLOCK_/queue/iosched/fifo_expire_async && read -p "Enter new value: " fifo_expire_async && echo $fifo_expire_async > /sys/block/$BLOCK_/queue/iosched/fifo_expire_async && C_P_Of_bfq;;
		9) echo "max_budget" && cat /sys/block/$BLOCK_/queue/iosched/max_budget && read -p "Enter new value: " max_budget && echo $max_budget > /sys/block/$BLOCK_/queue/iosched/max_budget && C_P_Of_bfq;;
		10) echo "strict_guarantees" && cat /sys/block/$BLOCK_/queue/iosched/strict_guarantees && read -p "Enter new value: " strict_guarantees && echo $strict_guarantees > /sys/block/$BLOCK_/queue/iosched/strict_guarantees && C_P_Of_bfq;;
		11) echo 16384 > /sys/block/$BLOCK_/queue/iosched/back_seek_max && echo 8 > /sys/block/$BLOCK_/queue/iosched/slice_idle && echo 124 > /sys/block/$BLOCK_/queue/iosched/timeout_sync && echo 2 > /sys/block/$BLOCK_/queue/iosched/back_seek_penalty && echo 1 > /sys/block/$BLOCK_/queue/iosched/low_latency && echo 8000 >/sys/block/$BLOCK_/queue/iosched/slice_idle_us && echo 250 > /sys/block/$BLOCK_/queue/iosched/fifo_expire_async && echo 0 > /sys/block/$BLOCK_/queue/iosched/max_budget  && echo 0 > /sys/block/$BLOCK_/queue/iosched/strict_guarantees && echo 125 > /sys/block/$BLOCK_/queue/iosched/fifo_expire_sync ;;
		12) main;;
	esac
	
}
##

## CHOOSING SCHEDULER TO CHANGE PARAMETERS
C_P_Of_Schedulers()
{
	read -p "Enter full name of scheduler: " S_To_Change
	case $S_To_Change in
		"mq-deadline") C_P_Of_mq_deadline;;
		"kyber") C_P_Of_kyber;;
		"bfq") C_P_Of_bfq;;
	esac
	
}
##

## CHANGE DEFAULT PARAMETRS
C_D_Scheduler()
{
	read -p "Enter full name of scheduler: " S_To_Change
	echo $S_To_Change > /sys/block/$BLOCK_/queue/scheduler
	A_S_After_Change
}
##


## MODULE SCHEDULERS
Schedulers()
{
 
	echo -e "${BPurple}\t\t\tI/O Schedulers menu\nChoose option:${Color_Off}\n\t${Green}1. View system information\n\t2. Available I/O Scheduler\n\t3. Change default scheduler\n\t4. Change scheduler parametrs ${BCyan}(before this step set default schedule which you want to change)${Color_Off}\n\t${Green}5. Start I/O Scheduler tests\n\t6. Back to the main menu\n\t7. Exit${Color_Off}"

	read -p "Enter 1-7: " Switch_Option
	case $Switch_Option in
		1) clear & I_B;;
		2) clear & A_S;;
		3) clear; C_D_Scheduler ;;
		4) clear ; C_P_Of_Schedulers;;
		5) clear & T_S;;
		6) clear & main;;
		7) clear ; exit;;
	esac
	Schedulers
}
##

## Available fstype
T_F()
{
	cd $work_dir/Filesystems/
	gcc test_write.c -o test_write.elf
	gcc test_read.c -o test_read.elf
	gcc test_create_files.c -o test_create_files.elf 
	gcc test_create_dirs.cpp -o  test_create_dirs.elf
	
	echo "Filesystems tests" |boxes -d tux -p a1 
	echo -e "${BYellow}Available filesystems${Color_Off}"
	for f in $(which /usr/bin/mkfs.*); do echo $f | awk -F '/usr/bin/mkfs.' '{print $2}'; done
	
	echo -e "${BYellow}Choose any filesystems to compare (you can compare more than 2 filesystems). Use space no comma: ${Color_Off}" 
	read -p "" filesystems
	
	read -ra fs_array <<< "$filesystems"
	
	for word in "${fs_array[@]}"
	do
		mkdir ./mnt_$word
		dd if=/dev/zero of=file_$word.bin bs=1M count=1024 status=progress
		##sudo chmod +rwx+rwx+rwx file_$word.bin;
	done
	  
	for word in "${fs_array[@]}"
	do
	  mkfs -t $word ./file_$word.bin
	done
	
	for word in "${fs_array[@]}"
	do
	  mount file_$word.bin ./mnt_$word
	done
	
	echo -e "${BPurple}TEST FILESYSTEM(S) TESTS START${Color_off}"
	
	echo -e "${Green}1. Writing"
	for word in "${fs_array[@]}"
	do
		echo -e "---> TEST --->" $word
		echo 3 > /proc/sys/vm/drop_caches
		##dd if=/dev/urandom of=./mnt_$word/data.bin bs=1M count=900
		echo 3 > /proc/sys/vm/drop_caches
		./test_write.elf ./mnt_$word/
	done
	
	echo -e "2. Reading"
	for word in "${fs_array[@]}"
	do
		echo -e "Test -->" $word
		echo 3 > /proc/sys/vm/drop_caches
		##dd if=./mnt_$word/data.bin of=/dev/null
		echo 3 > /proc/sys/vm/drop_caches
		./test_read.elf ./mnt_$word/
	done
	
	echo -e "3. Test file creating"
	for word in "${fs_array[@]}"
	do
		echo -e "Test -->" $word
		mkdir ./mnt_$word/dir/
		echo 3 > /proc/sys/vm/drop_caches
		ionice -c 1 ./test_create_files.elf ./mnt_$word/dir/
	done
	
	echo -e "4. Test directory creating${Color_Off}"
	for word in "${fs_array[@]}"
	do
		echo 3 > /proc/sys/vm/drop_caches
		ionice -c 1 ./test_create_dirs.elf ./mnt_$word/
	done
	
	read -p "Tests is over. Press Enter to delete all test's files and clear console: "
	for word in "${fs_array[@]}"
	do
		echo 3 > /proc/sys/vm/drop_caches
		umount file_$word.bin
		rm file_$word.bin
		rmdir ./mnt_$word
	done
	find . -name '*.elf' -delete
	find . -name '*.so' -delete
	cd $work_dir
}
##

## MODULE FILESYSTEMS
Filesystems()
{
	clear
	echo -e "${BPurple}\t\t\tFilesystems menu\nChoose option:${Color_Off}\n\t${Green}1. View system information\n\t2. Start filesystem tests\n\t3. Back to the main menu\n\t4. Exit${Color_Off}"

	read -p "Enter 1-4: " Switch_Option
	case $Switch_Option in
		1) clear & I_B;;
		2) clear ; T_F;;
		3) clear & main;;
		4) clear ; exit;;
	esac
	clear
	Filesystems
}
##

## Test Malloc/free function
Malloc_Free()
{
	cd $work_dir/MallocFree
	
	gcc malloc.c -o malloc.elf
	gcc -shared -fPIC mf_on_real_proc.c -o mf_on_real_proc.so
	gcc free.c -o free.elf
	gcc mfm.c -o mfm.elf
	
	echo "Malloc/Free tests" |boxes -d tux -p a1 
	echo -e "${BPurple}Malloc/Free tests start. Result will be in './log\'${Color_Off}"
	read -p "Enter buf for test: " buf
	mkdir logs
	echo -e "${Green}Test malloc${Color_Off}"
	./malloc.elf $buf > ./logs/malloc_result.csv
	echo -e "${Green}Test free${Color_Off}"
	./free.elf $buf > ./logs/free_result.csv
	echo -e "${Green}Test malloc->free->malloc again${Color_Off}"
	./mfm.elf $buf > ./logs/mfm_result.csv
	echo -e "${Green}Test malloc and free on real process${Color_Off}"
	LD_PRELOAD=./mf_on_real_proc.so ls
	echo -e "${BPurple}Malloc/Free tests end. Check result in log file (./logs/*)${Color_Off}"
	
	read -p "Do you want delete compiled files?(y/n): 	" response
	if [ "$response" == "y" ]; then
		find . -name '*.elf' -delete
		find . -name '*.so' -delete
	elif [ "$response" == "n" ]; then
		echo "Refuse delete"
	else
		echo "Uncorrect enter"
	fi
	
	
	cd $work_dir
	
}

print_art() 
{
	local frames=("frame1" "frame2" "frame3" "frame4" "frame5" "frame6" "frame7" "frame8" "frame9" "frame10" "frame11" "frame12" "frame13" "frame14" "frame15" "frame16" "frame17" "frame18" "frame19" "frame20" "frame21" "frame22" "frame23" "frame24" "frame25")  # Здесь замените на свои ASCII-арты или кадры анимации
	local delay=0.1  
	local frame_height=21 
    

	local terminal_height=$(tput lines)
    

	local menu_row=$((terminal_height - frame_height))
    
	while true; do
		clear  
        
        	for frame in "${frames[@]}"; do
        		#tput cup 0 0  
        		while IFS= read -r line;
        		do
                		echo "$line" 
            		done < "./frames/$frame"

	    		tput cup "$menu_row" 0  
	    		echo -e "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tOS" | figlet
	    		echo -e "${BGreen}Press Enter to stop animation and choose the number:${Color_Off}"
	    		echo -e "\t${Green}1. ForkBomb"
	   		echo -e "\t2. MemBomb"
			echo -e "\t3. Linpack"
			echo -e "\t4. I/O Scheduler"
			echo -e "\t5. Filesystems"
			echo -e "\t6. Malloc/Free"
			echo -e "\t7. Virtual Machines"
			echo -e "\t8. PAM-module"
			echo -e "\t9. Package-filter based on NetFilter interface"
			echo -e "\t10. A few words of thanks"
			echo -e "\t11. Help"
			echo -e "\t12. Author | Contacts"
			echo -e "\t13. Delete installed packages"
			echo -e "\t14. Exit${Color_Off}"
     		sleep "$delay" 
			if read -rsn1 -t 0.1; then
				break 2  
			fi
			clear  
		done  
    	done
}

LightMemBomb()
{
	cd $work_dir/MemBomb/LightMemBomb
	
	gcc -o main main.c
	gcc -o child child.c
	echo "project was compiled..."
	
	read -p "You have the opportunity to start the program now. 
	Be careful, it may damage your OS. 
	Alternatively, you can choose not to run the program right now and go to the MemBomb folder where the compiled files will be located. 
	Run - 'y', refuse to run - 'n':	" response
	if [ "$response" == "y" ]; then
		echo "Run..."
		./main
	elif [ "$response" == "n" ]; then
		echo "Refuse run..."
	else
		echo "Uncorrect enter"
	fi
	
	read -p "Do you want delete compiled files?(y/n): 	" response
	if [ "$response" == "y" ]; then
		find . -iname "main" -delete
		find . -iname "child" -delete
	elif [ "$response" == "n" ]; then
		echo "Refuse delete"
	else
		echo "Uncorrect enter"
	fi

	cd $work_dir
	clear;MemBombMenu
}

CrackLogin()
{
	cd $work_dir/MemBomb/CrackLogin
	gcc -o main main.c
	gcc -o child child.c
	echo "project was compiled..."
	
	read -p "You have the opportunity to start the program now. 
	Be careful, it may damage your OS. 
	Alternatively, you can choose not to run the program right now and go to the MemBomb folder where the compiled files will be located. 
	Run - 'y', refuse to run - 'n':	" response
	if [ "$response" == "y" ]; then
		echo "Run..."
		./main
	elif [ "$response" == "n" ]; then
		echo "Refuse run..."
	else
		echo "Uncorrect enter"
	fi
	
	read -p "Do you want delete compiled files?(y/n): 	" response
	if [ "$response" == "y" ]; then
		find . -iname "main" -delete
		find . -iname "child" -delete
	elif [ "$response" == "n" ]; then
		echo "Refuse delete"
	else
		echo "Uncorrect enter"
	fi

	cd $work_dir
	clear;MemBombMenu
	
}

KillLinux()
{
	cd $work_dir/MemBomb/KillLinux
	
	read -p "ATTENTION!
	The program will added to autorun,
	it is strongly not recommended to run it on the main OS. 
	Build - 'y', refuse build - 'n':	" response
	if [ "$response" == "y" ]; then
		echo "Build KillLinux..."
		gcc -o child child.c
		gcc -o main main.c

		echo "project was compiled..."
		
		read -p "You have the opportunity to start the program now. 
		Be careful, it may damage your OS. 
		Alternatively, you can choose not to run the program right now and go to the MemBomb folder where the compiled files will be located. 
		Run - 'y', refuse to run - 'n':	" response
		if [ "$response" == "y" ]; then
			echo "Run..."
			sudo touch /etc/systemd/system/KillLinux.service
			sudo chmod 644 /etc/systemd/system/KillLinux.service

			sudo tee /etc/systemd/system/KillLinux.service << EOF
[Unit]
Description=KillLinux
After=network.target

[Service]
ExecStart=$(find $work_dir/MemBomb/KillLinux -type f -name "main" -executable -print -quit)

[Install]
WantedBy=default.target
EOF
			sudo systemctl unmask KillLinux.service
			sudo systemctl daemon-reload
			sudo systemctl enable KillLinux.service
			sudo systemctl start KillLinux.service
			sudo ./main
		elif [ "$response" == "n" ]; then
			echo "Refuse run..."
		else
			echo "Uncorrect enter"
		fi
		
		read -p "Do you want delete compiled files?(y/n): 	" response
		if [ "$response" == "y" ]; then
			find . -iname "child" -delete
			find . -iname "main" -delete
			sudo rm -f /etc/systemd/system/KillLinux.service
		elif [ "$response" == "n" ]; then
			echo "Refuse delete"
		else
			echo "Uncorrect enter"
		fi
		
	elif [ "$response" == "n" ]; then
		echo "Refuse build..."
	else
		echo "Uncorrect enter"
	fi

	cd $work_dir
	clear;MemBombMenu
}

MooKillLinux()
{
	cd $work_dir/MemBomb/MooKillLinux
	
	read -p "ATTENTION!
	The program will added to autorun,
	it is strongly not recommended to run it on the main OS. 
	Build - 'y', refuse build - 'n':	" response
	if [ "$response" == "y" ]; then
		echo "Build MooKillLinux..."
		gcc -o child child.c
		gcc -o main main.c

		echo "project was compiled..."
		
		read -p "You have the opportunity to start the program now. 
		Be careful, it may damage your OS. 
		Alternatively, you can choose not to run the program right now and go to the MemBomb folder where the compiled files will be located. 
		Run - 'y', refuse to run - 'n':	" response
		if [ "$response" == "y" ]; then
			echo "Run..."
			sudo touch /etc/systemd/system/MooKillLinux.service
			sudo chmod 644 /etc/systemd/system/MooKillLinux.service

			sudo tee /etc/systemd/system/MooKillLinux.service << EOF
[Unit]
Description=MooKillLinux
After=network.target

[Service]
ExecStart=$(find $work_dir/MemBomb/MooKillLinux -type f -name "main" -executable -print -quit)

[Install]
WantedBy=default.target
EOF
			sudo systemctl unmask MooKillLinux.service
			sudo systemctl daemon-reload
			sudo systemctl enable MooKillLinux.service
			sudo systemctl start MooKillLinux.service
			sudo ./main
		elif [ "$response" == "n" ]; then
			echo "Refuse run..."
		else
			echo "Uncorrect enter"
		fi
		
		read -p "Do you want delete compiled files?(y/n): 	" response
		if [ "$response" == "y" ]; then
			find . -iname "child" -delete
			find . -iname "main" -delete
			sudo rm -f /etc/systemd/system/MooKillLinux.service
		elif [ "$response" == "n" ]; then
			echo "Refuse delete"
		else
			echo "Uncorrect enter"
		fi
		
	elif [ "$response" == "n" ]; then
		echo "Refuse build..."
	else
		echo "Uncorrect enter"
	fi

	cd $work_dir
	clear;MemBombMenu
}

MemBombMenu()
{
	echo -e "MemBombMenu \n\tP.S. Please check instructions before using!(It can damage your OS)" | boxes -d tux -p a1
	echo -e "\t${Green}1. Light MemBomb (Debian/Ubuntu/Arch Linux/Win64)"
	echo -e "\t2. CrackLogin (Debian/Ubuntu/Arch Linux)"
	echo -e "\t3. KillLinux (Debian/Ubuntu/Arch Linux)"
	echo -e "\t4. MooKillLinux (Debian/Ubuntu/Arch Linux)"
	echo -e "\t5. MemBombInstructions"
	echo -e "\t6. Go back${Color_Off}"
	
	
	read -p "Enter 1-6: " S_Modules
	case $S_Modules in
		1) clear; LightMemBomb;;
		2) clear; CrackLogin ;;
		3) clear;  KillLinux ;;
		4) clear; MooKillLinux;;      	 
		5) clear; main;;
	esac
	
}

ForkBomb()
{
	echo "ForkBomb" |boxes -d tux -p a1
	
	cd $BASEDIR/ForkBomb
	gcc -o main main.c
	
	read -p "You have the opportunity to start the program now. 
	Be careful, it may damage your OS. 
	Alternatively, you can choose not to run the program right now and go to the ForkBomb folder where the compiled files will be located. 
	The openprocess.c file should be compiled on Win64 (gcc -o openprocess openprocess.c). 
	Run - 'y', refuse to run - 'n':	" response
	if [ "$response" == "y" ]; then
		echo "Run..."
		sleep 2
		./main
	elif [ "$response" == "n" ]; then
		echo "Refuse run..."
	    
	else
		echo "Uncorrect enter"
	fi
	
	read -p "Do you want delete compiled files?(y/n): 	" response
	if [ "$response" == "y" ]; then
		find . -iname "main" -delete
	elif [ "$response" == "n" ]; then
		echo "Refuse delete"
	else
		echo "Uncorrect enter"
	fi
	
	cd $BASEDIR
	main
}


DetectVM()
{
	echo "Detect VM" |boxes -d tux -p a1
	
	cd $BASEDIR/VirtualMachines
	gcc -o vmdetect VMDetect.c
	
	read -p "You have the option to run the program right now.
	Alternatively, you can choose not to run the program right now and go to the VirtualMachines folder where the compiled files will be located. 
	Run - 'y', refuse to run - 'n':	" response
	if [ "$response" == "y" ]; then
		echo "Run..."
		./vmdetect
	elif [ "$response" == "n" ]; then
		echo "Refuse run..."
	else
		echo "Uncorrect enter"
	fi
	read -p "Do you want delete compiled files?(y/n): 	" response
	if [ "$response" == "y" ]; then
		find . -iname "hv_exit" -delete
	elif [ "$response" == "n" ]; then
		echo "Refuse delete"
	else
		echo "Uncorrect enter"
	fi
	clear
	cd $BASEDIR
	VirtualMachinesMenu
}

ExitFromVM()
{
	echo "Exit from VM" |boxes -d tux -p a1
	
	cd $BASEDIR/VirtualMachines
	nasm -f elf32 -o hv_exit.o hypervisor_exit.asm
	ld -m elf_i386 -o hv_exit hv_exit.o
	
	read -p "You have the option to run the program right now.
	Alternatively, you can choose not to run the program right now and go to the VirtualMachines folder where the compiled files will be located. If the program returns nothing, it means that the virtual machine is not detected.
	Run - 'y', refuse to run - 'n':	" response
	if [ "$response" == "y" ]; then
		echo "Run..."
		./hv_exit
	elif [ "$response" == "n" ]; then
		echo "Refuse run..."
	   
	else
		echo "Uncorrect enter"
	fi
	
	
	read -p "Do you want delete compiled files?(y/n): 	" response
	if [ "$response" == "y" ]; then
		find . -iname "hv_exit" -delete
	elif [ "$response" == "n" ]; then
		echo "Refuse delete"
	else
		echo "Uncorrect enter"
	fi
	
	cd $BASEDIR
	clear
	VirtualMachinesMenu
}

VirtualMachinesMenu()
{
	echo "VirtualMachines" |boxes -d tux -p a1
	echo -e "\t${Green}1. Detect VM"
	echo -e "\t2. Exit from VM (depends from core)"
	echo -e "\t3. Go back${Color_Off}"
	
	
	read -p "Enter 1-3: " S_Modules
	case $S_Modules in
		1) clear; DetectVM;;
		2) clear; ExitFromVM ;;
		3) clear; main;;
	esac
}

PamModule()
{
	echo "PAM-module" |boxes -d tux -p a1
	echo "This is an example of a simple pam-module where password hashing is applied using the crypto() function. After building the program, you will be able to either run the program or go to the project folder and get the built program from there.
	Default login/password: filter/admin"
	cd $BASEDIR/PAMModule
	
	case $os in
		debian|ubuntu)
		    
		gcc -g  -fPIC -shared pam_entry_auth.c -o pam_entry_auth.so   -lcrypt -ldl -lpam -lpam_misc 
		gcc -g entry_locker_auth.c -o  entry_locker_auth -ldl -lpam -lcrypt -lpam_misc
		
		sudo cp pam_entry_auth.so /lib/x86_64-linux-gnu/security/
		sudo touch /etc/pam.d/test_pam
		sudo chmod +xrw /etc/pam.d/test_pam
		sudo echo "auth required /lib/x86_64-linux-gnu/security/pam_entry_auth.so" > /etc/pam.d/test_pam
			
			;;
	 	arch)
		
		gcc -g  -fPIC -shared pam_entry_auth.c -o pam_entry_auth.so   -lcrypt -ldl -lpam -lpam_misc 
		gcc -g entry_locker_auth.c -o  entry_locker_auth -ldl -lpam -lcrypt -lpam_misc
		
		sudo cp pam_entry_auth.so /lib/security/
		sudo touch /etc/pam.d/test_pam
		sudo chmod +xrw /etc/pam.d/test_pam
		echo "auth required /lib/security/pam_entry_auth.so" > /etc/pam.d/test_pam
		    
		    ;;
		*)
		    echo "OS undetected"
		    exit 1
		    ;;
	esac

	read -p "Run - 'y', refuse to run - 'n':	" response
	if [ "$response" == "y" ]; then
		echo "Run..."
		./entry_locker_auth
	elif [ "$response" == "n" ]; then
		echo "Refuse run..."
	else
		echo "Uncorrect enter"
	fi
	
	read -p "Do you want delete compiled files?(y/n): " response
	if [ "$response" == "y" ]; then
		rm -f pam_entry_auth.so /lib/x86_64-linux-gnu/security/pam_entry_auth.so /etc/pam.d/test_pam
		rm -f pam_entry_locker_auth.so /lib/security/pam_entry_auth.so /etc/pam.d/test_pam
		rm -f pam_entry_auth
		rm -f entry_locker_auth
	elif [ "$response" == "n" ]; then
		echo "Refuse delete"
	else
		echo "Uncorrect enter"
	fi

	cd $BASEDIR
	clear
	main
}

Pleasures()
{
	echo "These words of gratitude are expressed 
	to my teachers S.V. Savkov and A.R. Khanov 
	for their timely support, 
	answers to questions (which I could get an answer to myself), 
	cheerful mood, interesting lectures and practices, 
	patience, approach to teaching us - non-trainers. 
	And also, for the absence of frameworks 
	and the possibility of creative approach in the realization of tasks." | boxes -d tux -p a4
	read -p "Press Enter to go back"
	main
}

FireWall()
{
	cd $BASEDIR/FireWall
	case $os in
		debian|ubuntu)
		    ./install.sh
		    ;;
		arch)
			sudo sh ./install.sh
			;;
		*)
		    echo "OS undetected"
		    exit 1
		    ;;
	esac
	
	echo "project was compiled..."
	echo "Firewall" |boxes -d tux -p a1
	echo "This is a simple example of a firewall implementation.
	It is installed in the autoloader and runs every time a user authenticates to the OS.
	Next, the superuser password must be entered to run the packet filter.
	IMPORTANT: There will be no access to the Internet if you do not authenticate."
	read -p "Do you want run firewall now? 
	Run - 'y', refuse to run - 'n':	" response
	if [ "$response" == "y" ]; then
		echo "Run..."
		./entry
	elif [ "$response" == "n" ]; then
		echo "Refuse run..."
	else
		echo "Uncorrect enter"
	fi
	
	read -p "Do you want delete compiled files?(y/n): 	" response
	if [ "$response" == "y" ]; then
		make clean
	elif [ "$response" == "n" ]; then
		echo "Refuse delete"
	else
		echo "Uncorrect enter"
	fi

	cd $BASEDIR
	main
}

main() 
{
	BASEDIR=$(dirname "$(realpath "$0")")    
	print_art 
	read -p "Enter 1-14: " S_Modules
	case $S_Modules in
		1) clear; ForkBomb ;;
		2) clear;  MemBombMenu ;;
		3) clear; Linpack;;
		4) BLOCK ; Schedulers;;
        	5) Filesystems;;
        	6) clear; Malloc_Free;;
        	7) cd $work_dir; clear; VirtualMachinesMenu ;;
        	8) clear; PamModule ;;
        	9) clear; FireWall ;;
		10) clear; Pleasures ;;       	 
		11) clear; InfoBlock;;
		12) clear; AuthorContacts;;
		13) clear; remove_programs;;
		14) clear; exit;;
	esac
	main
}

install_programs;
clear; cd $work_dir; main; echo -e "${Color_Off}"
