#bin/bash

# Init submodules
git submodule init
git submodule update
git submodule update --init --recursive

# Include ESP32 Snippets `cpp_utils`
if [ -d "/tmp/esp32-snippets" ]; then
		rm -rf "/tmp/esp32-snippets"
fi
git clone git@github.com:COM8/esp32-snippets.git "/tmp/esp32-snippets"
if [ -d "esp32/components" ]; then
		rm -rf "esp32/components"
fi
mkdir "esp32/components"
cp -r "/tmp/esp32-snippets/cpp_utils/" "esp32/components/cpp_utils"
rm -rf "/tmp/esp32-snippets/"