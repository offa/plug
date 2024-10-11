#!/bin/sh

usage() {
    cat <<EOF

Usage: 
    $0 enable  - enable non-root users to access all USB devices with Fender Mustang vendor id 1ed8
    $0 disable - revoke non-root user access if previously granted
Both variants require sudo access

EOF
}

udev_rules_filename=/etc/udev/rules.d/10-all_users_access_usb_fender_mustang.rules

if [ "$1" = "--help" ]
then

usage

exit 0

elif [ "$1" = "enable" ]
then

echo '

# Enable all users on this computer to interact with Fender Mustang devices with USB vendor id 1ed8
# Useful for contributors to https://github.com/offa/plug to enable them to experiment with 
# Fender devices not already supported.
SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device", ATTRS{idVendor}=="1ed8", GROUP="users"

' | sudo tee $udev_rules_filename > /dev/null

sudo udevadm control --reload

exit 0

elif [ "$1" = "disable" ]
then

sudo rm $udev_rules_filename

sudo udevadm control --reload

exit 0

else

usage

exit 1

fi