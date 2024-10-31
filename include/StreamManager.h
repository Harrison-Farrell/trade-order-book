#pragma once

// System Includes
#include <cstdint>
#include <fstream>
#include <map>
#include <string>

// Local Includes
#include "Messages.h"

namespace READER {
/// @brief The class is the entry and exit points for touching files.
/// The class opens three files.
/// - A single input file
/// - An output file which is used for exporting the snapshots
/// - An output file which logs the messages read in from the input file
class StreamManager {
 public:
  StreamManager() = delete;

  /// @brief Default destructor. Closes open files.
  ~StreamManager();

  /// @brief Default class constructor
  /// @param inPath Input Stream to read
  /// @param outpath Ouput Stream to write
  StreamManager(const std::string& inPath, const std::string& outpath);

  /// @brief Return the length of the input stream
  /// @return Size of stream
  size_t GetLength();

  /// @brief Get the current position of the cursor in the input file
  /// @return Position of the cursor
  size_t Tell();

  /// @brief Return input stream open status
  /// @return True if open
  bool IsOpen();

  /// @brief Decode the next message in the input stream
  /// @return Decoded Message struct
  const MSG::Message DecodeMessage();

  /// @brief Write information to the log Either 'STD::COUT' or Output file
  /// @param toTerminal flag to write to file or cout. True for terminal
  /// @param stream information stream to log
  void WriteLog(bool toTerminal, std::stringstream stream);

 private:
  // Three file managing objects
  std::ifstream reader_;
  std::ofstream ouput_writer_;
  std::ofstream debug_writer_;

  // Size of the input stream
  size_t input_stream_length_;

  // Sequence & Message - Debuggin only
  // Each newly decoded message is always logged.
  std::map<uint64_t, MSG::Message> messages_;
  void WriteLogEntry(const MSG::Message& msg);

  // Set initial input file length
  void SetLength();

  // Seek function to skip along the input file.
  // Used to seek over reserved message bytes
  StreamManager& Seek(std::streamoff offset, std::ios_base::seekdir way);

  // Read and return unsigned values from the input file
  uint8_t ReadUInt8();
  uint16_t ReadUInt16();
  uint32_t ReadUInt32();
  int64_t ReadInt64();

  // Read and return signed values from the input file
  int8_t ReadInt8();
  int16_t ReadInt16();
  int32_t ReadInt32();
  uint64_t ReadUInt64();

  // Read in bytes from the input file
  void Read1Byte(char* buffer);
  void Read2Bytes(char* buffer);
  void Read4Bytes(char* buffer);
  void Read8Bytes(char* buffer);
};
}  // namespace READER
