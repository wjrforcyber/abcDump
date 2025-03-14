[![.github/workflows/build-posix.yml](https://github.com/berkeley-abc/abc/actions/workflows/build-posix.yml/badge.svg)](https://github.com/berkeley-abc/abc/actions/workflows/build-posix.yml)
[![.github/workflows/build-windows.yml](https://github.com/berkeley-abc/abc/actions/workflows/build-windows.yml/badge.svg)](https://github.com/berkeley-abc/abc/actions/workflows/build-windows.yml)
[![.github/workflows/build-posix-cmake.yml](https://github.com/berkeley-abc/abc/actions/workflows/build-posix-cmake.yml/badge.svg)](https://github.com/berkeley-abc/abc/actions/workflows/build-posix-cmake.yml)


# ABCDump: Dump statistics from internal network

This repo has no new features on algorithms themselves but will give tiny examples on how to dump statistics efficiently from ABC.
<br/>The motivation is that data analysis in python is much more clear and powerful, but it's not easy to give visulization or analysis in C project. Some "stay out of the tool itself" commands can be used to retrieve the data to proper format and help aid the analysis.
<br/><em>Do notice that the show cases implemented here are quite simple and just helper commands for my own work, you should customize your own interface.</em>

## Build
I integrate the static library into the `CMakeList.txt` file, and due to some internet connection error, the GoogleTest package is removed manually since it sometimes failed downloading. So just:
```
make clean
mkdir build
cd build
cmake ..
make
```
And `abc` is built and you are good to go.

## Example
- `write_aig_json`
<br/>Write the aig information into `json` format, here I used an interesting library [json-c](https://github.com/json-c/json-c). There's also a quite clear tutorial [here](https://github.com/rbtylee/tutorial-jsonc).
<br/>There's a tiny example in `src/base/io/ioJsonAig.h` and `src/base/io/ioJsonAig.c`, you could customize the data you would like to collect.
<br/>For example:
```
abc 01> write_aig_json -h
usage: write_aig_json [-h] <file>
                 write the aig network in JSON format
        -h     : print the help message
        file   : the name of the file to write (extension .json)
abc 01> read_aiger i10.aig
abc 02> strash
abc 03> write_aig_json testonaig.json
The json representation:

{
  "DesignName":"i10",
  "LevelNum":50,
  "NodeNum":2675
}

testonaig.json saved.
abc 03> quit
```

<br/>*Note that the current version shows data like this due to my own need:
```
The json representation:

{
  "CINumber":257,
  "CONumber":224,
  "DesignName":"..\/i10",
  "LevelNum":50,
  "NodeNum":2675,
  "EdgesNum":4622,
  "NodeLevelInfo":[
    {
      "MaxLevel":50,
      "0% - 25%MaxLevel":1707,
      "25% - 50%MaxLevel":852,
      "50% - 75%MaxLevel":543,
      "75% - 100%MaxLevel":53
    }
  ],
  "FanoutInfo":[
    {
      "MaxFanout":75,
      "0% - 25%MaxFanout":2642,
      "25% - 50%MaxFanout":24,
      "50% - 75%MaxFanout":8,
      "0% - 100%MaxFanout":1
    }
  ]
}

testonaig.json saved.
```
<br/>Let's say there are tons of submodules in an unflatten design, after concatenate all the `json` data, you could do analysis using any cost function easily in python, here a plot of node number of top 50 cases after sorting submodule set by level is drawn:
1. Read and concatenate
```python
# By the courtesy of https://stackoverflow.com/a/75765186/19954247
import glob
import json
json_objects = []
target_path = "path/to/json/data/"
for f in glob.glob(target_path + "*preOpt.aig.json"):
    try:
        with open(f, "r", encoding='utf-8') as infile:
            file_content = json.load(infile)
            json_objects.append(file_content)
    except json.JSONDecodeError as e:
        print(f"Error {f}: {e}")

with open("merged_file.json", "w", encoding='utf-8') as outfile:
    json.dump(json_objects, outfile, ensure_ascii=False, indent=4)

```
2. Processing and plot
```python
import matplotlib
import pandas as pd
import os
with open("merged_file.json", "r", encoding='utf-8') as infile:
    file_content = json.load(infile)
df = pd.DataFrame(file_content)

df = df.sort_values("LevelNum", ascending=False)
df_new = df.head(50)

def splitDesignName(full_path):
    return os.path.basename(full_path)

df_design_base = df_new["DesignName"].copy().apply(splitDesignName)
df_new["DesignName"] = df_design_base

df_plot_NodeNum = pd.DataFrame({'NodeNum': df_new["NodeNum"].tolist()}, index=df_new["DesignName"].tolist())
ax = df_plot_NodeNum.plot.bar(rot=90)
```
<br/>Subdesign names are omitted.
<p align="center">
  <img src="showcase/bp.png" width="350"/>
</p>

# ABC: System for Sequential Logic Synthesis and Formal Verification

ABC is always changing but the current snapshot is believed to be stable.

## ABC fork with new features

Here is a [fork](https://github.com/yongshiwo/abc.git) of ABC containing Agdmap, a novel technology mapper for LUT-based FPGAs.  Agdmap is based on a technology mapping algorithm with adaptive gate decomposition [1]. It is a cut enumeration based mapping algorithm with bin packing for simultaneous wide gate decomposition, which is a patent pending technology.

The mapper is developed and maintained by Longfei Fan and Prof. Chang Wu at Fudan University in Shanghai, China.  The experimental results presented in [1] indicate that Agdmap can substantially improve area (by 10% or more) when compared against the best LUT mapping solutions in ABC, such as command "if".

The source code is provided for research and evaluation only. For commercial usage, please contact Prof. Chang Wu at wuchang@fudan.edu.cn.

References:

[1] L. Fan and C. Wu, "FPGA technology mapping with adaptive gate decompostion", ACM/SIGDA FPGA International Symposium on FPGAs, 2023. 

## Compiling:

To compile ABC as a binary, download and unzip the code, then type `make`.
To compile ABC as a static library, type `make libabc.a`.

When ABC is used as a static library, two additional procedures, `Abc_Start()` 
and `Abc_Stop()`, are provided for starting and quitting the ABC framework in 
the calling application. A simple demo program (file src/demo.c) shows how to 
create a stand-alone program performing DAG-aware AIG rewriting, by calling 
APIs of ABC compiled as a static library.

To build the demo program

 * Copy demo.c and libabc.a to the working directory
 * Run `gcc -Wall -g -c demo.c -o demo.o`
 * Run `g++ -g -o demo demo.o libabc.a -lm -ldl -lreadline -lpthread`

To run the demo program, give it a file with the logic network in AIGER or BLIF. For example:

    [...] ~/abc> demo i10.aig
    i10          : i/o =  257/  224  lat =    0  and =   2396  lev = 37
    i10          : i/o =  257/  224  lat =    0  and =   1851  lev = 35
    Networks are equivalent.
    Reading =   0.00 sec   Rewriting =   0.18 sec   Verification =   0.41 sec

The same can be produced by running the binary in the command-line mode:

    [...] ~/abc> ./abc
    UC Berkeley, ABC 1.01 (compiled Oct  6 2012 19:05:18)
    abc 01> r i10.aig; b; ps; b; rw -l; rw -lz; b; rw -lz; b; ps; cec
    i10          : i/o =  257/  224  lat =    0  and =   2396  lev = 37
    i10          : i/o =  257/  224  lat =    0  and =   1851  lev = 35
    Networks are equivalent.

or in the batch mode:

    [...] ~/abc> ./abc -c "r i10.aig; b; ps; b; rw -l; rw -lz; b; rw -lz; b; ps; cec"
    ABC command line: "r i10.aig; b; ps; b; rw -l; rw -lz; b; rw -lz; b; ps; cec".
    i10          : i/o =  257/  224  lat =    0  and =   2396  lev = 37
    i10          : i/o =  257/  224  lat =    0  and =   1851  lev = 35
    Networks are equivalent.

## Compiling as C or C++

The current version of ABC can be compiled with C compiler or C++ compiler.

 * To compile as C code (default): make sure that `CC=gcc` and `ABC_NAMESPACE` is not defined.
 * To compile as C++ code without namespaces: make sure that `CC=g++` and `ABC_NAMESPACE` is not defined.
 * To compile as C++ code with namespaces: make sure that `CC=g++` and `ABC_NAMESPACE` is set to
   the name of the requested namespace. For example, add `-DABC_NAMESPACE=xxx` to OPTFLAGS.

## Building a shared library

 * Compile the code as position-independent by adding `ABC_USE_PIC=1`.
 * Build the `libabc.so` target: 
 
     make ABC_USE_PIC=1 libabc.so

## Bug reporting:

Please try to reproduce all the reported bugs and unexpected features using the latest 
version of ABC available from https://github.com/berkeley-abc/abc

If the bug still persists, please provide the following information:    

 1. ABC version (when it was downloaded from GitHub)
 1. Linux distribution and version (32-bit or 64-bit)
 1. The exact command-line and error message when trying to run the tool
 1. The output of the `ldd` command run on the exeutable (e.g. `ldd abc`).
 1. Versions of relevant tools or packages used.


## Troubleshooting:

 1. If compilation does not start because of the cyclic dependency check, 
try touching all files as follows: `find ./ -type f -exec touch "{}" \;`
 1. If compilation fails because readline is missing, install 'readline' library or
compile with `make ABC_USE_NO_READLINE=1`
 1. If compilation fails because pthreads are missing, install 'pthread' library or
compile with `make ABC_USE_NO_PTHREADS=1`
    * See http://sourceware.org/pthreads-win32/ for pthreads on Windows
    * Precompiled DLLs are available from ftp://sourceware.org/pub/pthreads-win32/dll-latest
 1. If compilation fails in file "src/base/main/libSupport.c", try the following:
    * Remove "src/base/main/libSupport.c" from "src/base/main/module.make"
    * Comment out calls to `Libs_Init()` and `Libs_End()` in "src/base/main/mainInit.c"
 1. On some systems, readline requires adding '-lcurses' to Makefile.

The following comment was added by Krish Sundaresan:

"I found that the code does compile correctly on Solaris if gcc is used (instead of 
g++ that I was using for some reason). Also readline which is not available by default 
on most Sol10 systems, needs to be installed. I downloaded the readline-5.2 package 
from sunfreeware.com and installed it locally. Also modified CFLAGS to add the local 
include files for readline and LIBS to add the local libreadline.a. Perhaps you can 
add these steps in the readme to help folks compiling this on Solaris."

The following tutorial is kindly offered by Ana Petkovska from EPFL:
https://www.dropbox.com/s/qrl9svlf0ylxy8p/ABC_GettingStarted.pdf

## Final remarks:

Unfortunately, there is no comprehensive regression test. Good luck!                                

This system is maintained by Alan Mishchenko <alanmi@berkeley.edu>. Consider also 
using ZZ framework developed by Niklas Een: https://bitbucket.org/niklaseen/abc-zz (or https://github.com/berkeley-abc/abc-zz)
