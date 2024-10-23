// System Includes
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

// Local Includes
#include "Book.h"
#include "Messages.h"
#include "StreamManager.h"

int main(int argc, char* argv[])
{
  // Default completion to unsuccessful non-zero value
  int execution_status = 1;

  // Default input values
  int depth = 10;
  // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
  std::string input_path = "./../dependency/input2.stream";
  std::string output_path = "output.snapshot";

  // Simply defaulting to if passed arugments are incorrect.
  if(argc == 2)
  {
    depth = std::stoi(argv[1]);
    std::cout << "The Depth supplied is: " << depth << std::endl;
    std::cout << "Defaulting input stream to " << input_path << std::endl;
    std::cout << "Defaulting input stream to " << output_path << std::endl;
  }
  else if(argc > 2)
  {
    std::cout << "Too many arguments supplied.\n Defaulting to Depth of " << depth << std::endl;
    std::cout << "Defaulting input stream to " << input_path << std::endl;
    std::cout << "Defaulting input stream to " << output_path << std::endl;
  }
  else
  {
    std::cout << "To fewer arguments supplied.\n Defaulting to Depth of " << depth << std::endl;
    std::cout << "Defaulting input stream to " << input_path << std::endl;
    std::cout << "Defaulting input stream to " << output_path << std::endl;
  }

  try
  {
    DATA::Book book(depth);

    // Read in the binary file
    READER::StreamManager reader(input_path, output_path);
    if(reader.IsOpen())
    {
      std::cout << "Open file Successful" << std::endl;

      while(reader.Tell() < reader.GetLength())
      {
        MSG::Message msg = reader.DecodeMessage();
        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
        reader.WriteLog(false, book.UpdateBook(msg));
      }

      // Set execution_status to 0 - successful
      execution_status = 0;
    }
    else
    {
      std::cout << "Open file Failed." << std::endl;
    }
  }
  // Catch any unexpected errors and print
  catch(std::exception& e)
  {
    std::cerr << "exeception caught: " << e.what() << std::endl;
    // return from main non zero result.
  }

  return execution_status;
}