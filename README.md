# Uranium profiler
Uranium is fast and lightweight instrumentation profiler for Rust written in Rust and C++.

## Building
The only way to use Uranium is to build it from source.

Supported platforms:<br>
✔ - The module was tested on this platform.
|  Module  | Windows | Linux | MacOS |
| -------- |:-------:|:-----:|:-----:|
| Frontend | ✔       |       |       |
| Backend  | ✔       | ✔     |       |

Please note that currently the backend works only with x86-64 CPUs due to usage of specific instructions (see ![tracking issue for this](https://github.com/UraniumTeam/uranium-profiler/issues/11))

#### Prerequisites
To build and run *frontend*:
 - **CMake v1.17.0** minimum
 - **Qt v6.0.0** minimum

To build *backend*:
 - **Rust** (Tested with v1.61, may not work with older versions)

#### Clone the repository
```shell
git clone https://github.com/UraniumTeam/uranium-profiler
cd uranium-profiler
```
#### Build the backend
```shell
cd uranium-backend
cargo build
cd ../uranium-prof-macro
cargo build
```
#### Build the frontend
```shell
cd ../uranium-visualizer
cmake -S . -B BuildRelease -DCMAKE_BUILD_TYPE=Release
```
#### Run sample program
```shell
cd ../backend-manual-tests
cargo run
```
Now you should see `uranium_session_bin.ups` file and `uranium_session_bin` directory that store data collected by the profier's backend during
the time the sample program was running. You can now open the '.ups' file in profiler frontend.
