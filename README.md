# HeliosXSimulator
HeliosXSimulator is a verilator-based Soc Simulator for HeliosXCore.

## Usage
```
git clone git@github.com:HeliosXCore/HeliosXSimulator.git
cd HeliosXSimulator
git submodule update --init --recursive

# Build 3rd-party library
# Build fmt
cd 3rd-party/fmt && mkdir build && cd build && cmake -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE .. && make
# Build HeliosXEmulator
cd ../../HeliosXEmulator && make static

# Build and Run example
cd ../../ && make example
```
