#pragma once
#include "Order.h"

class Node
{
public:
  Node(Order& order, Node* prev, Node* next)
  : order{ order },
  prev{ prev },
  next{ next }
  { }

  Order order;
  Node* prev;
  Node* next;
};
