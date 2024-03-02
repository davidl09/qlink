#!/bin/bash

# Check if the script is being run as root
if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

# File containing required packages
PKG_FILE="required_pkgs.txt"

# Check if the package file exists
if [ ! -f "$PKG_FILE" ]; then
    echo "Package file $PKG_FILE not found."
    exit 1
fi

# Update package lists
apt update

# Install packages listed in the file
while IFS= read -r package
do
    # Check if the package is already installed
    if dpkg -l | grep -q -E "^ii\s+$package\s"; then
        echo "$package is already installed."
    else
        # Install the package
        apt install -y "$package"
    fi
done < "$PKG_FILE"

echo "Installing sqlpp11"

git clone https://github.com/rbock/sqlpp11
cd sqlpp11 || exit
mkdir build
cd build || echo "Error: could not create build directory"
cmake -DBUILD_SQLITE3_CONNECTOR=ON -DDEPENDENCY_CHECK=ON -DBUILD_TESTING=OFF ..


cd ../..
sudo rm -r  sqlpp11
echo "Successfully installed sqlpp11 system-wide"

echo "Package installation completed."



