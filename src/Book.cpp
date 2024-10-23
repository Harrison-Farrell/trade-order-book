// System Includes
#include <cstring>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// Local Includes
#include "Book.h"
#include "Messages.h"

DATA::Book::Book(int depth)
  : depth_(depth)
{}

// Creating the unique ID to be used a key for the order log
const std::string DATA::Book::CreateKey(MSG::Message& msg)
{
  std::string unquie_id;
  unquie_id.append(msg.id.symbol);
  unquie_id += msg.id.side;
  unquie_id += std::to_string(msg.id.order_id);
  return unquie_id;
}

// Creating the unique ID to be used a key for the order log
const std::string DATA::Book::CreateKey(MSG::Order& order)
{
  std::string unquie_id;
  unquie_id.append(order.symbol);
  unquie_id += order.side;
  unquie_id += std::to_string(order.id);
  return unquie_id;
}

// Function to determine what computation should occur for the incoming message
// Updates both the order log & the snapshot data
std::stringstream DATA::Book::UpdateBook(MSG::Message& msg)
{
  // Check the message type
  switch(msg.m_type)
  {
  case MSG::MSG_TYPE::eADDED: {
    // Create new order entry
    MSG::Order order;
    strcpy_s(order.symbol, msg.id.symbol);
    order.id = msg.id.order_id;
    order.side = msg.id.side;
    order.price = msg.m_msg.m_update.price;
    order.volume = msg.m_msg.m_update.volume;
    // Add entry into the Snapshot map
    SnapshotAdd(order);
    // Add entry into the log with Unqiue ID
    order_log_.try_emplace(CreateKey(msg), order);
    break;
  }
  case MSG::MSG_TYPE::eUPDATE: {
    // Find Unqiue ID from the log
    auto item = order_log_.find(CreateKey(msg));
    // Update the snapshot first before updating the order log
    SnapshotUpdate(item->second, msg.m_msg.m_update.price, msg.m_msg.m_update.volume);
    // Update changes to the order log
    item->second.price = msg.m_msg.m_update.price;
    item->second.volume = msg.m_msg.m_update.volume;
    break;
  }
  case MSG::MSG_TYPE::eDELETE: {
    // Find Unqiue ID from the log
    auto item = order_log_.find(CreateKey(msg));
    // Remove order from snapshot
    SnapshotDelete(item->second);
    // Remove order from order log
    order_log_.erase(item);
    break;
  }
  case MSG::MSG_TYPE::eEXECUTE: {
    // Find Unqiue ID from the log
    auto item = order_log_.find(CreateKey(msg));
    // Update snapshot
    SnapshotExecute(item->second, msg.m_msg.m_execute.traded);
    // Update order log
    item->second.volume -= msg.m_msg.m_execute.traded;
    // Check if the order is still valid.
    if(item->second.volume == 0)
    {
      order_log_.erase(item);
    }
    break;
  }
  }

  // Return a string stream of the snapshot group effected by the inbound message
  return PrintSnapshot(msg.m_sequence_id, std::string(msg.id.symbol));
}

void DATA::Book::SnapshotAdd(MSG::Order& order)
{
  // Check if the snapshot grouping exists already
  auto it_snapshot = snapshots_.find(std::string(order.symbol));
  // If the grouping does exist
  if(it_snapshot != snapshots_.end())
  {
    // The order is on the buy side
    if(order.side == 'B')
    {
      // try emplace the order into the snapshot grouping buy side
      it_snapshot->second.first.try_emplace(order.price, order.volume);
    }
    // The order is on the sell side
    else
    {
      // try emplace the order into the snapshot grouping buy side
      it_snapshot->second.second.try_emplace(order.price, order.volume);
    }
  }
  // Order Snapshot doesn't exist. Creating new snapshot grouping
  else
  {
    std::map<int32_t, uint64_t> buy;
    std::map<int32_t, uint64_t> sell;
    // Populating either the buy or sell side initially
    // The other side will contain an empty map.
    if(order.side == 'B')
    {
      buy.try_emplace(order.price, order.volume);
    }
    else
    {
      sell.try_emplace(order.price, order.volume);
    }
    // try emplace the order into the newly created snapshot grouping buy & sell side
    snapshots_.try_emplace(std::string(order.symbol), std::make_pair(buy, sell));
  }
}

void DATA::Book::SnapshotUpdate(MSG::Order& order, int32_t& price, uint64_t& volume)
{
  auto it_snapshot = snapshots_.find(order.symbol);
  // The order is on the buy side
  if(order.side == 'B')
  {
    // Check if the price & volume has changed or just the volume
    if(price != order.price)
    {
      // Moving the volume from the old price to the new price

      // Iterator to the price element in the buy map
      auto price_it = it_snapshot->second.first.find(order.price);
      // Remove the old volume from the price snapshot
      price_it->second -= order.volume;
      // Total volume at that price is now zero. Remove from map
      if(price_it->second == 0)
      {
        it_snapshot->second.first.erase(price_it);
      }

      // Add the new price and volume to the snapshot
      price_it = it_snapshot->second.first.find(price);
      // The price entry already is in the map
      if(price_it != it_snapshot->second.first.end())
      {
        // Add the volume to the price
        price_it->second += volume;
      }
      else
      {
        // try emplace the new price and volume into the snapshot
        it_snapshot->second.first.try_emplace(price, volume);
      }
    }
    // No change in price, only volume
    else
    {
      // Iterator to the price element in the buy map
      auto price_it = it_snapshot->second.first.find(order.price);
      // Previous order was larger.
      if(order.volume > volume)
      {
        // Add the new volume diffence to the price snapshot
        price_it->second += (order.volume - volume);
      }
      // Previous order was smaller.
      else
      {
        // remove the new volume diffence to the price snapshot
        price_it->second -= (order.volume - volume);
      }
      // Total volume at that price is now zero. Remove from map
      if(price_it->second == 0)
      {
        it_snapshot->second.first.erase(price_it);
      }
    }
  }
  // Sell Side
  else
  {
    // Moving the volume from the old price to the new price Sell Map
    if(price != order.price)
    {
      // Iterator to the price element in the sell map
      auto price_it = it_snapshot->second.second.find(order.price);
      // Remove the old volume frmo the price snapshot
      price_it->second -= order.volume;
      // Total volume at that price is now zero. Remove from map
      if(price_it->second == 0)
      {
        it_snapshot->second.second.erase(price_it);
      }

      // Add the new price and volume to the snapshot
      price_it = it_snapshot->second.second.find(price);
      // The price entry already is in the map
      if(price_it != it_snapshot->second.second.end())
      {
        // Add the volume to the price
        price_it->second += volume;
      }
      else
      {
        // Emplace the new price and volume into the snapshot map
        it_snapshot->second.second.try_emplace(price, volume);
      }
    }
    // No change in price, only volume
    else
    {
      // Iterator to the price element in the buy map
      auto price_it = it_snapshot->second.second.find(order.price);
      // Previous order was larger.
      if(order.volume > volume)
      {
        // Add the new volume diffence to the price snapshot
        price_it->second += (order.volume - volume);
      }
      // Previous order was smaller.
      else
      {
        // remove the new volume diffence to the price snapshot
        price_it->second -= (order.volume - volume);
      }
      // Total volume at that price is now zero. Remove from map
      if(price_it->second == 0)
      {
        it_snapshot->second.second.erase(price_it);
      }
    }
  }
}

void DATA::Book::SnapshotDelete(MSG::Order& order)
{
  auto it_snapshot = snapshots_.find(std::string(order.symbol));
  if(order.side == 'B')
  {
    // Removing the order from the snapshot
    auto price_it = it_snapshot->second.first.find(order.price);

    // Volume is a unsigned value. Could wrap
    if(order.volume >= price_it->second)
    {
      price_it->second = 0;
    }
    else
    {
      price_it->second -= order.volume;
    }

    // The total volume is now 0. No longer needed in the snapshot
    if(price_it->second <= 0)
    {
      it_snapshot->second.first.erase(price_it);
    }
  }
  // Sell Side
  else
  {
    // Removing the order from the snapshot
    auto price_it = it_snapshot->second.second.find(order.price);

    // Volume is a unsigned value. Could wrap
    if(order.volume >= price_it->second)
    {
      price_it->second = 0;
    }
    else
    {
      price_it->second -= order.volume;
    }
    // The total volume is now 0. No longer needed in the snapshot
    if(price_it->second <= 0)
    {
      it_snapshot->second.second.erase(price_it);
    }
  }
}

void DATA::Book::SnapshotExecute(MSG::Order& order, uint64_t& volume)
{
  auto it_snapshot = snapshots_.find(order.symbol);
  if(order.side == 'B')
  {
    // Iterator to the price element in the buy map
    auto price_it = it_snapshot->second.first.find(order.price);

    // Volume is a unsigned value. Could wrap
    if(volume >= price_it->second)
    {
      price_it->second = 0;
    }
    else
    {
      price_it->second -= volume;
    }

    // Total volume at that price is now zero. Remove from map
    if(price_it->second == 0)
    {
      it_snapshot->second.first.erase(price_it);
    }
  }
  else
  {
    // Iterator to the price element in the sell map
    auto price_it = it_snapshot->second.second.find(order.price);

    // Volume is a unsigned value. Could wrap
    if(volume >= price_it->second)
    {
      price_it->second = 0;
    }
    else
    {
      price_it->second -= volume;
    }
    // Total volume at that price is now zero. Remove from map
    if(price_it->second == 0)
    {
      it_snapshot->second.second.erase(price_it);
    }
  }
}

void DATA::Book::PrintBook()
{
  for(auto log : order_log_)
  {
    std::cout << "\t--- Key: " << log.first << "---\n" << log.second << std::endl;
  }
}

std::stringstream DATA::Book::PrintSnapshot(uint64_t& sequence, const std::string symbol)
{
  std::stringstream output;
  if(!snapshots_.empty())
  {
    std::vector<std::string> buy_array;
    std::vector<std::string> sell_array;

    auto it_snapshot = snapshots_.find(symbol);

    output << sequence << ", " << symbol << ", ";

    int buy_depth = 0;
    int sell_depth = 0;

    // Creating the Buy objects to be logged if Within the depth
    for(auto const& [key, val] : it_snapshot->second.first)
    {
      // The Map is an ordered map. Therefore the Key 'price' increases.
      // The first element is the smallest value
      if(buy_depth < depth_)
      {
        buy_array.push_back("(" + std::to_string(key) + ", " + std::to_string(val) + ")");
        buy_depth++;
      }
      else
      {
        // The depth has been reached, break the loop early
        break;
      }
    }

    // Creating the Sell objects to be logged if Within the depth
    for(auto const& [key, val] : it_snapshot->second.second)
    {
      // The Map is an ordered map. Therefore the Key 'price' increases.
      // The first element is the smallest value
      if(sell_depth < depth_)
      {
        sell_array.push_back("(" + std::to_string(key) + ", " + std::to_string(val) + ")");
        sell_depth++;
      }
      else
      {
        // The depth has been reached, break the loop early
        break;
      }
    }

    // Appending all the buy objects into a single stream.
    output << "[";
    for(int i = 0; i < buy_array.size(); i++)
    {
      // reverse the array highest to lowest
      output << buy_array[buy_array.size() - 1 - i];
      // Inserting the coma seperator for entities but the last
      if(i < buy_array.size() - 1)
      {
        output << ", ";
      }
    }
    output << "]";

    // Appending all the sell objects into a single stream.
    output << ", [";
    for(int i = 0; i < sell_array.size(); i++)
    {
      // reverse the array highest to lowest
      output << sell_array[sell_array.size() - 1 - i];
      // inserting the coma seperator for entities but the last
      if(i < sell_array.size() - 1)
      {
        output << ", ";
      }
    }
    output << "]" << std::endl;
  }
  return output;
}