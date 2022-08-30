
#include "OrderCache.h"

#include <iostream>

// add order to the cache
void OrderCache::addOrder(Order order)
{
  Node* node = new Node(order, nullptr, nullptr);

  if (m_head == nullptr)
  {
    m_head = node;
    m_tail = node;
  }
  else
  {
    m_tail->next = node;
    node->prev = m_tail;
    m_tail = node;
  }

  m_mapOrderId.insert(std::pair<std::string, Node*>(order.orderId(), node));
  addOrderToUserMap(order);
  addOrderToSecurityMap(order);
}

// remove order with this unique order id from the cache
void OrderCache::cancelOrder(const std::string& orderId)
{
  if (m_mapOrderId.find(orderId) == m_mapOrderId.end())
  {
    return;
  }

  Node* node = m_mapOrderId[orderId];

  if (node == m_head && node == m_tail)
  {
    m_head = nullptr;
    m_tail = nullptr;
  }
  else if (node->prev == nullptr)
  {
    m_head = node->next;
  }
  else if(node->next == nullptr && node->prev == m_head)
  {
    m_tail = node->prev;
  }

  Node* prev = node->prev;
  if (prev != nullptr)
  {
    prev->next = node->next;
  }

  if (node->next != nullptr)
  {
    node->next->prev = prev;
  }

  m_mapOrderId.erase(orderId);
  m_mapUser[node->order.user()].erase(orderId);
  m_mapSecurity[node->order.securityId()].erase(orderId);

  delete node;
}

// remove all orders in the cache for this user
void OrderCache::cancelOrdersForUser(const std::string& user)
{
  if(m_mapUser.find(user) == m_mapUser.end())
  {
    return;
  }

  auto orderIds = m_mapUser[user];
  for(auto& orderId : orderIds)
  {
    cancelOrder(orderId);
  }
  m_mapUser.erase(user);
}

// remove all orders in the cache for this security with qty >= minQty
void OrderCache::cancelOrdersForSecIdWithMinimumQty(const std::string& securityId, unsigned int minQty)
{
  if(m_mapSecurity.find(securityId) == m_mapSecurity.end())
  {
    return;
  }

  auto orderIds = m_mapSecurity[securityId];
  for(auto& orderId : orderIds)
  {
    Order* order = &m_mapOrderId[orderId]->order;
    if (order->qty() >= minQty)
    {
      cancelOrder(orderId);
    }
  }
  if (m_mapSecurity[securityId].size() == 0)
  {
    m_mapSecurity.erase(securityId);
  }
}

// return the total qty that can match for the security id
unsigned int OrderCache::getMatchingSizeForSecurity(const std::string& securityId)
{
  if(m_mapSecurity.find(securityId) == m_mapSecurity.end())
  {
    return 0;
  }

  unsigned int matchingSize = 0;
  auto& orderIds = m_mapSecurity[securityId];
  auto buyOrders = getBuyOrdersIds(orderIds);
  auto sellOrders = getSellOrders(orderIds);

  for (auto& sellOrder : sellOrders)
  {
    matchBuysAndSells(sellOrder, buyOrders, matchingSize);
  }
  return matchingSize;
}

// return all orders in cache in a vector
std::vector<Order> OrderCache::getAllOrders() const
{
  std::vector<Order> result;
  Node* temp = m_head;
  while(temp->next != nullptr)
  {
    result.push_back(temp->order);
    temp = temp->next;
  }
  result.push_back(temp->order);
  return result;
}

// add order to the security map
void OrderCache::addOrderToSecurityMap(const Order& order)
{
  if (m_mapSecurity.find(order.securityId()) != m_mapSecurity.end())
  {
    auto& orderIds = m_mapSecurity[order.securityId()];
    orderIds.insert(order.orderId());
  }
  else
  {
    m_mapSecurity.insert( { order.securityId(), { order.orderId() } });
  }
}

// add order to the user map
void OrderCache::addOrderToUserMap(const Order& order)
{
  if (m_mapUser.find(order.user()) != m_mapUser.end())
  {
    auto& orderIds = m_mapUser[order.user()];
    orderIds.insert(order.orderId());
  }
  else
  {
    m_mapUser.insert( { order.user(), { order.orderId() } });
  }
}

// get all buy ordersIds from the orders map
std::unordered_map<std::string, unsigned int> OrderCache::getBuyOrdersIds(const std::unordered_set<std::string>& orderIds)
{
  std::unordered_map<std::string, unsigned int> buys_map;

  for(auto& orderId : orderIds)
  {
    Order* order = &m_mapOrderId[orderId]->order;

    if (order->side() == "Buy")
    {
      if (buys_map.find(order->company()) != buys_map.end())
      {
        auto& accumulatedBuy = buys_map[order->company()];
        accumulatedBuy += order->qty();
      }
      else
      {
        buys_map[order->company()] = order->qty();
      }
    }
  }
  return buys_map;
}

// get all sell orders objects from the orders map
std::unordered_map<std::string, unsigned int> OrderCache::getSellOrders(const std::unordered_set<std::string>& orderIds)
{
  std::unordered_map<std::string, unsigned int> sells_map;

  for(auto orderId : orderIds)
  {
    Order* order = &m_mapOrderId[orderId]->order;
    if (order->side() == "Sell")
    {
      if (sells_map.find(order->company()) != sells_map.end())
      {
        auto& accumulatedSell = sells_map[order->company()];
        accumulatedSell += order->qty();
      }
      else
      {
        sells_map[order->company()] = order->qty();
      }
    }
  }

  return sells_map;
}

// match available sells with buy requests
void OrderCache::matchBuysAndSells(std::pair<std::string, unsigned int> sell, std::unordered_map<std::string, unsigned int>& buysOrderIds, unsigned int& matchingSize)
{
  for(auto& buy : buysOrderIds)
  {
    if(sell.first == buy.first)
    {
      continue;
    }

    if (sell.second <= buy.second)
    {
      matchingSize += sell.second;
      buy.second -= sell.second;
    }
    else
    {
      matchingSize += buy.second;
      buy.second = 0;
    }
  }
}
