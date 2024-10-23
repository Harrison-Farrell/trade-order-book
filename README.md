# Trade Order Book Programming task

More information to the programing task found in `dependency/Programming Task_Order Book Feed Processing.pdf`

Author: Harrison Farrell\
Email: H.Thomas.Farrell@Gmail.com

### Description
An order book lists the number of shares being bid on or offered at each price point, or market depth. It also identifies the market participants behind the buy and sell orders, though some choose to remain anonymous. These lists help traders and also improve market transparency because they provide valuable trading information. 

This is a programming task to take an input file of order book feeds and produce price depth snapshots in a human readable format

The first section, Order Book & Price Depth , details what an order book and a price depth are and how they relate to each other. This will be helpful to understand what it means to handle an order book feed and produce a price depth snapshot.

The second section, Task Description , defines the scope of the task and necessary details on what an order book is and what a price depth is. Knowledge of these structures is required to understand and build an appropriate solution, which is described in the section prior.

The final section, Messages , is the reference material describing the set of possible messages within the input file. Information includes the data types and the order of fields for each message. 1


### Master Assumptions:
    1 - The input stream is always correct. There isn't a need to check for incomplete messages.

    2 - The processing of the snapshots isn't time sensetive. Single threaded meets the requirements.

    3 - Messages are always in order. Can't get an execute / delete before an Add / Update.

    4 - The snapshot depth is applied to both side. Buy & Sell sides have the same depth.

    5 - User inputs such as command line argumes and file paths are expected to be correct.

    
### Notes:
    1 - The executable is expecting a single value as a command line argument. It will default to a depth of 5 if anything more or less is provided. Won't error check. The provided argument

    2 - Lines 21 & 22 of the main 'order_book.cpp' is where the input.stream and output.snapshot are set.

    3 - Line 60 of the main 'order_book.cpp' is where the output of terminal or file is selected. Set reader.WriteLog bool argument to either true or terminal logging, false for file logging.

    4 - The debugging input message log is created next to the executable 'Debug.log' A function of the StreamManager

### Dependencies:
    CMake version 3.20
    GCC version 6.3
    Has been built on Linux

### Build:
    The project has been built using a Cmake & GCC. The steps to run the executable.
    1 - From the top level directory. Create a directory. 'build'

    2 - Navigate into the newly created build directory and from a terminal call cmake. 'cmake ..'

    3 - Once the files have been generated. Call 'cmake --build .' or 'make'

    4 - Complete.