#pragma once

#include "Node.h"
#include "OrderCacheInterface.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>


class OrderCache : public OrderCacheInterface
{

public:
  // add order to the cache
  void addOrder(Order order) override; // Object

  // remove order with this unique order id from the cache
  void cancelOrder(const std::string& orderId) override; // Order ID

  // remove all orders in the cache for this user
  void cancelOrdersForUser(const std::string& user) override; // User

  // remove all orders in the cache for this security with qty >= minQty
  void cancelOrdersForSecIdWithMinimumQty(const std::string& securityId, unsigned int minQty) override; // SecurityID

  // return the total qty that can match for the security id
  unsigned int getMatchingSizeForSecurity(const std::string& securityId) override;

  // return all orders in cache in a vector
  std::vector<Order> getAllOrders() const override;

private:
  // add order to the security map
  void addOrderToSecurityMap(const Order& order);

  // add order to the user map
  void addOrderToUserMap(const Order& order);

  // get all buy ordersIds from the orders map
  std::unordered_map<std::string, unsigned int> getBuyOrdersIds(const std::unordered_set<std::string>& orderIds);

  // get all sell orders objects from the orders map
  std::unordered_map<std::string, unsigned int> getSellOrders(const std::unordered_set<std::string>& orderIds);

  // match available sells with buy requests
  void matchBuysAndSells(std::pair<std::string, unsigned int> sell, std::unordered_map<std::string, unsigned int>& buysOrderIds, unsigned int& matchSize);

private:
  Node* m_head;
  Node* m_tail;

  // HashMap for storing the Orders
  std::unordered_map<std::string, Node*> m_mapOrderId;
  // HashMap for storing the Users and its corresponding Order
  std::unordered_map<std::string, std::unordered_set<std::string>> m_mapUser;
  // HashMap for storing the SecurirtyIds and its corresponding Order
  std::unordered_map<std::string, std::unordered_set<std::string>> m_mapSecurity;
};
