# Trade Order Book Programming task

More information to the programing task found in `dependency/Programming Task_Order Book Feed Processing.pdf`

Author: Harrison Farrell\
Email: H.Thomas.Farrell@Gmail.com

### Dependencies:
    Linux version Ubunutu 24.04.1 LTS
    CMake version 3.28.3
    GCC version 13.2.0

### Master Assumptions:
    1 - The input stream is always correct. There isn't a need to validate for incomplete messages.

    2 - The processing of the snapshots isn't time sensetive. Single threaded meets the requirements.

    3 - Messages are always in order. Can't get an execute / delete before an Add / Update has happend.

    4 - The snapshot depth is applied to both side. Buy & Sell sides have the same depth.

    5 - User inputs such as command line argumes and file paths are expected to be correct.

### Build steps:
    Assuming all dependences are met. The steps to run the executable as as follows.

    1 - From the top level directory. Create a build directory.
    2 - Navigate into the newly created build directory and generate the cmake build files.
    3 - Once the files have been generated. Call 'cmake --build .' or 'make'
    4 - Run the execute. Either by passing 0, 1, or 3 arguments
            0 arguments:
                    Depth: 5
                    Input: ./../dependency/input2.stream
                    Output: output.snapshot

            1 argument:
                    Depth: user defined.
                    Input: ./../dependency/input2.stream
                    Output: output.snapshot

            3 arguments:
                    Depth: user defined.
                    Input: user defined.
                    Output: user defined.

    mkdir build  
    cd build/
    cmake ..
    cmake --build .
    ./order_book 5
    
### Notes:
    1 - Lines 21 & 22 of the main 'order_book.cpp' is where the input.stream and output.snapshot defaults are set.

    2 - Line 60 of the main 'order_book.cpp' can sent where the output is sent. Either to terminal or file. THrough the `reader.WriteLog` bool argument. True = terminal. False = file.

    3 - The debugging input message log is created next to the executable 'Debug.log' A function of the StreamManager

