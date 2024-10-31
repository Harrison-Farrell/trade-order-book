// System Inlcudes
#include <map>
#include <sstream>

// Local Include
#include "Messages.h"
#include "StreamManager.h"

READER::StreamManager::~StreamManager() {
  reader_.close();
  ouput_writer_.close();
  debug_writer_.close();
}

READER::StreamManager::StreamManager(const std::string& filePath,
                                     const std::string& outpath) {
  this->reader_ =
    std::ifstream(filePath, std::ifstream::in | std::ifstream::binary);

  this->ouput_writer_ =
    std::ofstream(outpath, std::ofstream::out | std::ofstream::trunc);

  this->debug_writer_ =
    std::ofstream("Debug.log", std::ofstream::out | std::ofstream::trunc);

  if (this->reader_.fail()) throw std::runtime_error("File does not exist");

  this->SetLength();
  this->Seek(0, std::ios_base::beg);
}

size_t READER::StreamManager::GetLength() { return this->input_stream_length_; }

READER::StreamManager& READER::StreamManager::Seek(std::streamoff offset,
                                                   std::ios_base::seekdir way) {
  reader_.seekg(offset, way);
  return *this;
}

bool READER::StreamManager::IsOpen() { return reader_.is_open(); }

size_t READER::StreamManager::Tell() { return (size_t)this->reader_.tellg(); }

void READER::StreamManager::SetLength() {
  this->Seek(0, std::ios_base::end);
  this->input_stream_length_ = this->Tell();
}

void READER::StreamManager::Read1Byte(char* buffer) {
  this->reader_.read(buffer, 1);
}

void READER::StreamManager::Read2Bytes(char* buffer) {
  this->reader_.read(buffer, 2);
}

void READER::StreamManager::Read4Bytes(char* buffer) {
  this->reader_.read(buffer, 4);
}

void READER::StreamManager::Read8Bytes(char* buffer) {
  this->reader_.read(buffer, 8);
}

uint8_t READER::StreamManager::ReadUInt8() {
  uint8_t data_packet;
  Read1Byte(reinterpret_cast<char*>(&data_packet));
  return data_packet;
}

int8_t READER::StreamManager::ReadInt8() {
  uint8_t data_packet;
  Read1Byte(reinterpret_cast<char*>(&data_packet));
  return data_packet;
}

uint16_t READER::StreamManager::ReadUInt16() {
  uint16_t data_packet;
  Read2Bytes(reinterpret_cast<char*>(&data_packet));
  return data_packet;
}

int16_t READER::StreamManager::ReadInt16() {
  int16_t data_packet;
  Read2Bytes(reinterpret_cast<char*>(&data_packet));
  return data_packet;
}

uint32_t READER::StreamManager::ReadUInt32() {
  uint32_t data_packet;
  Read4Bytes(reinterpret_cast<char*>(&data_packet));
  return data_packet;
}

int32_t READER::StreamManager::ReadInt32() {
  int32_t data_packet;
  Read4Bytes(reinterpret_cast<char*>(&data_packet));
  return data_packet;
}

uint64_t READER::StreamManager::ReadUInt64() {
  uint64_t data_packet;
  Read8Bytes(reinterpret_cast<char*>(&data_packet));
  return data_packet;
}

int64_t READER::StreamManager::ReadInt64() {
  int64_t data_packet;
  Read8Bytes(reinterpret_cast<char*>(&data_packet));
  return data_packet;
}

const MSG::Message READER::StreamManager::DecodeMessage() {
  MSG::Message incoming_message;

  incoming_message.m_sequence_id = ReadUInt32();
  // Seeking over the size
  Seek(4, std::ios_base::cur);

  switch ((char)ReadUInt8()) {
    case 'A':
      incoming_message.m_type = MSG::MSG_TYPE::eADDED;
      break;
    case 'U':
      incoming_message.m_type = MSG::MSG_TYPE::eUPDATE;
      break;
    case 'D':
      incoming_message.m_type = MSG::MSG_TYPE::eDELETE;
      break;
    case 'E':
      incoming_message.m_type = MSG::MSG_TYPE::eEXECUTE;
      break;
  };

  incoming_message.id.symbol[0] = (char)ReadUInt8();  // Always 'V'
  incoming_message.id.symbol[1] = (char)ReadUInt8();  // Always 'C'
  incoming_message.id.symbol[2] = (char)ReadUInt8();  // 0 - 9
  incoming_message.id.symbol[3] = '\0';  // Setting the string to end in null
  incoming_message.id.order_id = ReadUInt64();
  incoming_message.id.side = (char)ReadUInt8();

  switch (incoming_message.m_type) {
    case MSG::MSG_TYPE::eADDED:
      Seek(3, std::ios_base::cur);
      incoming_message.m_msg.m_update.volume = ReadUInt64();
      incoming_message.m_msg.m_update.price = ReadInt32();
      Seek(4, std::ios_base::cur);
      break;

    case MSG::MSG_TYPE::eUPDATE:
      Seek(3, std::ios_base::cur);
      incoming_message.m_msg.m_update.volume = ReadUInt64();
      incoming_message.m_msg.m_update.price = ReadInt32();
      Seek(4, std::ios_base::cur);
      break;

    case MSG::MSG_TYPE::eDELETE:
      Seek(3, std::ios_base::cur);
      break;

    case MSG::MSG_TYPE::eEXECUTE:
      Seek(3, std::ios_base::cur);
      incoming_message.m_msg.m_execute.traded = ReadUInt64();
      break;
  }

  WriteLogEntry(incoming_message);
  return incoming_message;
}

void READER::StreamManager::WriteLogEntry(const MSG::Message& msg) {
  messages_.emplace(msg.m_sequence_id, msg);
  debug_writer_ << msg;
}

void READER::StreamManager::WriteLog(bool toTerminal,
                                     std::stringstream stream) {
  if (toTerminal) {
    std::cout << stream.str() << std::endl;
  } else {
    ouput_writer_ << stream.str();
  }
}
