#!/bin/bash

# get cimage
wget https://cimage.ermiry.com/download/latest/linux64 -O postman.tar.gz
sudo tar -xzf postman.tar.gz -C /opt
rm postman.tar.gz
sudo ln -s /opt/Postman/Postman /usr/bin/postman

# # adding icon launcher
cp /opt/cimage/cimage.desktop /home/$USER/.local/share/applications/