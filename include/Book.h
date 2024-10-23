#pragma once

// System Includes
#include <map>
#include <string>
#include <utility>

// Local Includes
#include "Messages.h"

namespace DATA
{
  /// @brief The Book class holds all the data read in from the input file.
  /// The Book is responsible for performing all the computation on the inbound messages.
  class Book
  {
    public:
    /// @brief Default class constructor
    /// @param depth snapshots depth to be used
    Book(int depth);

    /// @brief Function to update the order log & snapshot information export
    /// @param msg Message to update the logs with
    /// @return snapshot information export
    std::stringstream UpdateBook(MSG::Message& msg);

    /// @brief Print the order log Debugging funtion.
    void PrintBook();

    private:
    // Depth of the snapshot information
    int depth_;

    // Map of all orders being tracked [Unique ID : Order message]
    std::map<std::string, MSG::Order> order_log_;

    // Map of all snapshots being tracked
    // [Symbol : Pair]
    // The pair is a pair of maps the [buy : sell] sides
    // each map within the buy, sell sides is price & volume [price : volume]
    std::map<std::string, std::pair<std::map<int32_t, uint64_t>, std::map<int32_t, uint64_t>>>
      snapshots_;

    // functions to create unique IDs per order.  [Symbol + Side + ID]
    const std::string CreateKey(MSG::Message& msg);
    const std::string CreateKey(MSG::Order& order);

    // Functions to compute how the snaphots should be modifed per each new message
    void SnapshotAdd(MSG::Order& order);
    void SnapshotUpdate(MSG::Order& order, int32_t& price, uint64_t& volume);
    void SnapshotDelete(MSG::Order& order);
    void SnapshotExecute(MSG::Order& order, uint64_t& volume);

    // Print a snapshot given a ID symbol
    std::stringstream PrintSnapshot(uint64_t& sequence, const std::string symbol);
  };
} // namespace DATA