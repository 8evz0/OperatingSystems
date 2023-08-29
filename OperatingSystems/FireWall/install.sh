#!/bin/bash

# Поменять названия 

sudo apt-get install --only-upgrade bash
chmod +x install.sh
chmod +x menu_script.sh
os=$(grep -oP '(?<=^ID=).+' /etc/os-release | tr -d '"')

case $os in
    debian|ubuntu)
    	packages=(
	    ["figlet"]="figlet"
	    ["boxes"]="boxes"
	    ["tmux"]="tmux"
	    ["cowsay"]="cowsay"
	    ["lolcat"]="lolcat"
	    ["libnfnetlink"]="libnfnetlink"
	    ["libnfnetlink-dev"]="libnfnetlink-dev"
	    ["libnetfilter-queue-dev"]="libnetfilter-queue-dev"
	    ["libnet-dev"]="libnet-dev"
	    ["iptables"]="iptables"
	    ["libcurl4-openssl-dev"]="libcurl4-openssl-dev"
	    ["libxml2-dev"]="libxml2-dev"
	    #["pip"]="pip"
	    ["git"]="git"
	    #["pycurl"]="pycurl"
	)
	sudo killall apt-get
        for package in "${!packages[@]}"; do
            if ! dpkg -s "${packages[$package]}" >/dev/null 2>&1; then
                sudo apt-get install -y "${packages[$package]}"
            else
                echo "$package is already installed."
            fi
        done

    make clean && make all

	########### ПУСК ОЧЕРЕДИ ЧЕРЕЗ СЕРВИС (ФИЛЬТР-ПАКЕТОВ) ############
		
	sudo iptables -A INPUT -j NFQUEUE --queue-num 0
	sudo iptables -A OUTPUT -j NFQUEUE --queue-num 0
	
	###########  АВТОЗАГРУЗКА СЕРВИСА ############
	sudo touch /etc/systemd/system/entry.service
	sudo chmod 644 /etc/systemd/system/entry.service
		
	sudo tee /etc/systemd/system/entry.service << EOF
[Unit]
Description=entry
After=entry.target

[Service]
ExecStart=$(find $PWD -type f -name "entry" -executable -print -quit)

[Install]
WantedBy=default.target
EOF
	program_path=$(find $PWD -type f -name "entry" -executable -print -quit)
	
	# Определяем, какой файл использовать для автозапуска (bashrc или zshrc)
	autostart_file=~/.bashrc
	if [ -n "$ZSH_VERSION" ]; then
	  autostart_file=~/.zshrc
	fi

	# Проверяем, что строка уже не существует в файле
	if ! grep -q "export PATH=\$PATH:$program_path" "$autostart_file"; then
	  # Добавляем путь до программы в конец файла с новой строкой
	  echo "export PATH=\$PATH:$program_path" >> "$autostart_file"
	  echo "Путь до программы добавлен в $autostart_file."
	else
	  echo "Путь до программы уже присутствует в $autostart_file."
	fi
	

	sudo systemctl unmask entry.service
	sudo systemctl daemon-reload
	sudo systemctl enable entry.service
	sudo systemctl start entry.service
	
      	
      	mkdir -p ~/.config/autostart/
	touch ~/.config/autostart/entry.desktop
	sudo tee ~/.config/autostart/entry.desktop << EOF
[Desktop Entry]
Name=SHIELD
Exec=x-terminal-emulator -e $program_path
Type=Application
EOF

        ;;
    arch)
    	declare -A packages=(
	    ["figlet"]="figlet"
	    ["boxes"]="boxes"
	    ["tmux"]="tmux"
	    ["cowsay"]="cowsay"
	    ["lolcat"]="lolcat"
	    ["libnfnetlink"]="libnfnetlink"
	    ["libnfnetlink-dev"]="libnfnetlink-dev"
	    ["libnetfilter-queue-dev"]="libnetfilter-queue-dev"
	    ["iptables"]="iptables"
	    ["libcurl4-openssl-dev"]="libcurl4-openssl-dev"
	    ["libxml2-dev"]="libxml2-dev"
	    ["libnet"]="libnet"
	)
    	sudo killall pacman
        for package in "${!packages[@]}"; do
            if ! pacman -Qs "${packages[$package]}" >/dev/null 2>&1; then
                sudo pacman -S "${packages[$package]}"
            else
                echo "$package is already installed."
            fi
        done
        
        sudo pacman -S pam base-devel pam-utils 

        # Устанавливаем yaourt, если он еще не установлен
        if ! pacman -Qs yaourt >/dev/null 2>&1; then
            sudo pacman -S --needed base-devel git wget yajl
            cd /tmp
            git clone https://aur.archlinux.org/package-query.git
            cd package-query/
            makepkg -si && cd /tmp/
            git clone https://aur.archlinux.org/yaourt.git
            cd yaourt/
            makepkg -si
            current_user=$(logname)
            sudo -u "$current_user" yaourt -S boxes
        else
            echo "yaourt is already installed."
        fi
        
        sudo make clean && sudo make all
       
        
        ########### ПУСК ОЧЕРЕДИ ЧЕРЕЗ СЕРВИС (ФИЛЬТР-ПАКЕТОВ) ############
		
	sudo iptables -A INPUT -j NFQUEUE --queue-num 0
	sudo iptables -A OUTPUT -j NFQUEUE --queue-num 0
	#iptables-save > /etc/iptables/rules.v4
	#ip6tables-save > /etc/iptables/rules.v6
		
	###########  АВТОЗАГРУЗКА СЕРВИСА ############
	sudo touch /etc/systemd/system/entry.service
	sudo chmod 644 /etc/systemd/system/entry.service
		
	sudo tee /etc/systemd/system/entry.service << EOF
[Unit]
Description=entry
After=entry.target

[Service]
ExecStart=$(find / -type f -name "entry" -executable -print -quit)

[Install]
WantedBy=default.target
EOF

	program_path=$(find / -type f -name "entry" -executable -print -quit)
	
	# Определяем, какой файл использовать для автозапуска (bashrc или zshrc)

	autostart_file="/etc/profile"

	# Проверяем, что строка уже не существует в файле
	if ! grep -q "export PATH=\$PATH:$program_path" "$autostart_file"; then
	  # Добавляем путь до программы в конец файла с новой строкой
	  echo "export PATH=\$PATH:$program_path" >> "$autostart_file"
	  echo "Путь до программы добавлен в $autostart_file."
	else
	  echo "Путь до программы уже присутствует в $autostart_file."
	fi

	
	sudo systemctl unmask entry.service
	sudo systemctl daemon-reload
	sudo systemctl enable entry.service
	sudo systemctl start entry.service
	
	
	mkdir -p ~/.config/autostart/
	touch ~/.config/autostart/entry.desktop
	sudo tee ~/.config/autostart/entry.desktop << EOF
[Desktop Entry]
Name=SHIELD
Exec=$program_path
Terminal=true
Type=Application
X-GNOME-Autostart-enabled=true
EOF
	
        ;;
    *)
        echo "The package manager for this operating system could not be detected."
        exit 1
        ;;
esac
