#pragma once

#include "Order.h"

class OrderCacheInterface
{

public:
  // implememnt the 6 methods below, do not alter signatures

  // add order to the cache
  virtual void addOrder(Order order) = 0; // Object

  // remove order with this unique order id from the cache
  virtual void cancelOrder(const std::string& orderId) = 0; // Order ID

  // remove all orders in the cache for this user
  virtual void cancelOrdersForUser(const std::string& user) = 0; // User

  // remove all orders in the cache for this security with qty >= minQty
  virtual void cancelOrdersForSecIdWithMinimumQty(const std::string& securityId, unsigned int minQty) = 0; // SecurityID

  // return the total qty that can match for the security id
  virtual unsigned int getMatchingSizeForSecurity(const std::string& securityId) = 0;

  // return all orders in cache in a vector
  virtual std::vector<Order> getAllOrders() const = 0;
};
