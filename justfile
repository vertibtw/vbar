binary := "vbar"

build:
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build

debug:
    cmake -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build

clean:
    rm -rf build

run: build
    ./build/{{binary}}

run-debug: debug
    ./build/{{binary}}
