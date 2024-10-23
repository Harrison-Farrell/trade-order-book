#pragma once

// System Includes
#include <cstdint>
#include <iostream>
#include <string>

namespace MSG
{
  /// @brief Enumeration of the type of messages.
  enum MSG_TYPE
  {
    eADDED,
    eUPDATE,
    eDELETE,
    eEXECUTE
  };

  /// @brief Struct containing variables shared between all messages
  typedef struct
  {
    // 3 charater char arry. Last element is NULL '\0'
    char symbol[4];
    // The unique order id. Only unique for the VC group & side.
    uint64_t order_id;
    // Side of the order Either 'B' or 'S' for Buy & Sell
    char side;
  } Msg_Header;

  /// @brief Struct containing the extra variables for the update message
  typedef struct
  {
    // Volume of the trade
    uint64_t volume;
    // Price of the trade
    int32_t price;
  } Msg_Update;

  /// @brief Struct containing the extra variables for the execute message
  typedef struct
  {
    // Volume of the traded
    uint64_t traded;
  } Msg_Execute;

  /// @brief Union to seperate the information of the messages
  typedef union
  {
    Msg_Update m_update;
    Msg_Execute m_execute;
  } Msg_Union;

  /// @brief Main message struct
  typedef struct
  {
    // Sequence the message was received
    uint64_t m_sequence_id;
    Msg_Header id;
    MSG_TYPE m_type;
    Msg_Union m_msg;
  } Message;

  /// @brief Main Order struct
  typedef struct
  {
    char symbol[4];
    uint64_t id;
    char side;
    int32_t price;
    uint64_t volume;
  } Order;


// --- --- --- Inline Overloading functions for debugging & printing --- --- ---

  inline std::ostream& operator<<(std::ostream& os, const Msg_Header msg)
  {
    os << "Symbol: " << msg.symbol << "\tSide: " << msg.side << "\tID: " << msg.order_id
       << std::endl;
    return os;
  }

  inline std::ostream& operator<<(std::ostream& os, const Msg_Update msg)
  {
    os << "Price: " << msg.price << "\tVolume: " << msg.volume << std::endl;
    return os;
  }

  inline std::ostream& operator<<(std::ostream& os, const Msg_Execute msg)
  {
    os << "Traded: " << msg.traded << std::endl;
    return os;
  }

  inline std::ostream& operator<<(std::ostream& os, const Message msg)
  {
    os << "--- Sequence: " << msg.m_sequence_id << " ---\n" << msg.id << "Type: ";

    switch(msg.m_type)
    {
    case MSG_TYPE::eADDED:
      os << "A";
      break;
    case MSG_TYPE::eUPDATE:
      os << "U";
      break;
    case MSG_TYPE::eDELETE:
      os << "D";
      break;
    case MSG_TYPE::eEXECUTE:
      os << "E";
      break;
    }

    os << "\t";

    switch(msg.m_type)
    {
    case MSG_TYPE::eADDED:
      os << msg.m_msg.m_update;
      break;
    case MSG_TYPE::eUPDATE:
      os << msg.m_msg.m_update;
      break;
    case MSG_TYPE::eDELETE:
      os << "\n";
      break;
    case MSG_TYPE::eEXECUTE:
      os << msg.m_msg.m_execute;
      break;
    }

    os << "\n";

    return os;
  }

  inline std::ostream& operator<<(std::ostream& os, const Order msg)
  {
    os << "Symbol: " << msg.symbol << "\tSide: " << msg.side << "\tID: " << msg.id
       << "\tPrice: " << msg.price << "\tVoluem: " << msg.volume << std::endl;
    return os;
  }

} // namespace MSG