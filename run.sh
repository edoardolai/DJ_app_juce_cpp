# Build 
mkdir -p build
cd build
cmake ..
make -j$(sysctl -n hw.ncpu)  # Use all available cores

# Run 
cd ..
./build/OtoDecks_artefacts/OtoDecks.app/Contents/MacOS/OtoDecks