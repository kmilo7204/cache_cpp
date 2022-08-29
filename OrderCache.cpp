// https://www.geeksforgeeks.org/design-a-data-structure-for-lru-cache/
#include "OrderCache.h"
#include "Node.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <vector>
#include <climits>
#include <cstdlib>


class Cache
{
// Default constructor

public:
    Node* head;
    Node* tail;

    std::unordered_map<std::string, Node*> mapOrderId;
    // This one only gets a reference to order IDs.
    std::unordered_map<std::string, std::unordered_set<std::string>> mapUser;
    // Key -> User, Value -> { orders }
    std::unordered_map<std::string, std::unordered_set<std::string>> mapSecurity;

public:
    void addOrder(Order order)
    {
        // New node with both prev and next as nullptr
        Node* node = new Node(order, nullptr, nullptr);

        if (head == nullptr)
        {
            // First entry will be the head and the tail
            head = node;
            tail = node;
        }
        else
        {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }

        mapOrderId.insert(std::pair<std::string, Node*>(order.orderId(), node));
        addToUserMap(order);
        addToSecurityMap(order);
    }

    void getUserOrders(std::string user)
    {
        auto res = mapUser[user];
        std::cout << res.size() << std::endl;
    }

    void printUserMap()
    {
        for(auto user : mapUser)
        {
            std::cout << "User: " << user.first << std::endl;
            // std::cout << "Order size: " << user.second.size() << std::endl;
            for (auto order : user.second)
            {
                std::cout << "---> Order: " << order << std::endl;
            }
        }
    }

    void printSecurityMap()
    {
        for(auto user : mapSecurity)
        {
            std::cout << "Secuirty: " << user.first << std::endl;
            // std::cout << "Order size: " << user.second.size() << std::endl;
            for (auto order : user.second)
            {
                std::cout << "---> Order: " << order << std::endl;
            }
        }
    }

    void printOrders()
    {
        for(auto user : mapOrderId)
        {
            std::cout << "OrderId: " << user.first << std::endl;
        }
    }

    void cancelOrder(const std::string& orderId)
    {
        if (mapOrderId.find(orderId) == mapOrderId.end())
        {
            return;
        }

        Node* node = mapOrderId[orderId];
        Node* prev = node->prev;

        if (prev != nullptr)
        {
            prev->next = node->next;
        }

        if (node->next != nullptr)
        {
            node->next->prev = prev;
        }

        // Erase all from the maps
        Order* order = &node->order;
        mapOrderId.erase(orderId);
        mapUser[order->user()].erase(orderId);
        mapSecurity[order->securityId()].erase(orderId);

        delete node;
    }

    void cancelOrdersForUser(const std::string& user)
    {
        if(mapUser.find(user) == mapUser.end())
        {
            return;
        }

        auto orderIds = mapUser[user];
        for(auto& orderId : orderIds)
        {
            cancelOrder(orderId);
        }
        mapUser.erase(user);
    }

    void cancelOrdersForSecIdWithMinimumQty(const std::string& securityId, unsigned int minQty)
    {
        if(mapSecurity.find(securityId) == mapSecurity.end())
        {
            std::cout << "securityId is not in the cache" << std::endl;
            return;
        }
        auto orderIds = mapSecurity[securityId];
        for(auto& orderId : orderIds)
        {
            Order* order = &mapOrderId[orderId]->order;
            if (order->qty() >= minQty)
            {
                cancelOrder(orderId);
            }
        }
        // No more security IDS associates
        if (mapSecurity[securityId].size() == 0)
        {
            mapSecurity.erase(securityId);
        }
    }

    void addToUserMap(Order& order)
    {
        if (mapUser.find(order.user()) != mapUser.end())
        {
            auto& orderIds = mapUser[order.user()];
            orderIds.insert(order.orderId());
        }
        else
        {
            mapUser.insert( { order.user(), { order.orderId() } });
        }
    }

    void addToSecurityMap(Order& order)
    {
        if (mapSecurity.find(order.securityId()) != mapSecurity.end())
        {
            auto& orderIds = mapSecurity[order.securityId()];
            orderIds.insert(order.orderId());
        }
        else
        {
            mapSecurity.insert( { order.securityId(), { order.orderId() } });
        }
    }

    std::vector<Order> getAllOrders()
    {
        std::vector<Order> result;
        Node* temp = head;
        while(temp->next != nullptr)
        {
            result.push_back(temp->order);
            temp = temp->next;
        }
        result.push_back(temp->order);
        return result;
    }

    unsigned int getMatchingSizeForSecurity(const std::string& securityId)
    {
        if(mapSecurity.find(securityId) == mapSecurity.end())
        {
            return 0;
        }

        unsigned int matchSize = 0;
        auto orderIds = mapSecurity[securityId];
        auto buyOrders = getBuyOrdersIds(orderIds);
        auto sellOrders = getSellOrders(orderIds);

        for (auto& sellOrder : sellOrders)
        {
            getUnUsedBuysMap(sellOrder, buyOrders, matchSize);
        }
        return matchSize;
    }

    void getUnUsedBuysMap(std::pair<std::string, unsigned int> sell,
        std::unordered_map<std::string, unsigned int>& buysOrderIds, unsigned int& matchSize)
    {
        for(auto& buy : buysOrderIds)
        {
            if(sell.first == buy.first)
            {
                continue;
            }

            if (sell.second <= buy.second)
            {
                matchSize += sell.second;
                buy.second -= sell.second;
            }
            else
            {
                matchSize += buy.second;
                buy.second = 0;
            }

            // matchSize += std::abs((int)sell.second - (int)buy.second);
        }
    }

    void getUnUsedBuys(Order& sell, std::unordered_set<std::string>& buysOrderIds, unsigned int& matchSize)
    {
        // Since I create a copy I can modify that value
        // Take the order sell value
        auto replyOrderIds = buysOrderIds;
        int remaining = sell.qty();
        for (auto buy: buysOrderIds)
        {
            Order* buyOrder = &mapOrderId[buy]->order;
            // std::cout << buyOrder->orderId() + " " + buyOrder->company() << std::endl;
            // std::cout << sell.orderId() + " " + sell.company() << std::endl;
            if ((sell.company() == buyOrder->company()) || remaining < buyOrder->qty())
            {
                continue;
            }
            remaining -= buyOrder->qty();
            matchSize += buyOrder->qty();
            replyOrderIds.erase(buy);
            // buysOrderIds.erase(buy);
            if (remaining == 0)
            {
                return;
            }
        }
        buysOrderIds = replyOrderIds;
    }

    std::unordered_map<std::string, unsigned int> getBuyOrdersIds(const std::unordered_set<std::string>& orderIds)
    {
        std::unordered_set<std::string> buys;
        std::unordered_map<std::string, unsigned int> buys_map;

        // std::cout << "getBuyOrdersIds" << std::endl;
        // std::cout << orderIds.size() << std::endl;
        for(auto& orderId : orderIds)
        {
            Order* order = &mapOrderId[orderId]->order;

            // std::cout << "Checking OrderId: " << order->orderId() << std::endl;
            if (order->side() == "Buy")
            {
                //Security and the quantity
                if (buys_map.find(order->company()) != buys_map.end())
                {
                    // std::cout << "Company is in the map: " << order->company() << std::endl;
                    auto& accumulatedBuy = buys_map[order->company()];
                    accumulatedBuy += order->qty();
                }
                else
                {
                    // std::cout << "Inserting" << std::endl;
                    buys_map[order->company()] = order->qty();
                }
                // std::cout << "Adding: " << order->orderId() << std::endl;
                buys.insert(orderId);
            }
        }

        std::cout << "Buys" << std::endl;
        for(auto acc : buys_map)
        {
            std::cout << acc.first + ": " << acc.second << std::endl;
        }
        // return buys;
        return buys_map;
    }

    std::unordered_map<std::string, unsigned int> getSellOrders(const std::unordered_set<std::string>& orderIds)
    {
        // I get here the complete order, because I will compare it later
        std::vector<Order> sells;
        std::unordered_map<std::string, unsigned int> sells_map;

        for(auto orderId : orderIds)
        {
            Order* order = &mapOrderId[orderId]->order;
            if (order->side() == "Sell")
            {
                //Security and the quantity
                if (sells_map.find(order->company()) != sells_map.end())
                {
                    auto& accumulatedSell = sells_map[order->company()];
                    accumulatedSell += order->qty();
                }
                else
                {
                    // std::cout << "Inserting" << std::endl;
                    sells_map[order->company()] = order->qty();
                }

                sells.push_back(*order);
            }
        }

        std::cout << "Sells" << std::endl;
        for(auto acc : sells_map)
        {
            std::cout << acc.first + ": " << acc.second << std::endl;
        }
        // return sells;
        return sells_map;
    }
};

int main()
{
    std::cout << "Starting" << std::endl;
    Cache my_cache{};

    // Order order_1 = Order("OrdId1", "SecId1", "Buy", 1000, "User1", "CompanyA");
    // Order order_2 = Order("OrdId2", "SecId2", "Sell", 3000, "User2", "CompanyB");
    // Order order_3 = Order("OrdId3", "SecId1", "Sell", 500, "User3", "CompanyA");
    // Order order_4 = Order("OrdId4", "SecId2", "Buy", 600, "User4", "CompanyC");
    // Order order_5 = Order("OrdId5", "SecId2", "Buy", 100, "User5", "CompanyB");
    // Order order_6 = Order("OrdId6", "SecId3", "Buy", 1000, "User6", "CompanyD");
    // Order order_7 = Order("OrdId7", "SecId2", "Buy", 2000, "User7", "CompanyE");
    // Order order_8 = Order("OrdId8", "SecId2", "Sell", 5000, "User8", "CompanyE");

    // my_cache.addOrder(order_1);
    // my_cache.addOrder(order_2);
    // my_cache.addOrder(order_3);
    // my_cache.addOrder(order_4);
    // my_cache.addOrder(order_5);
    // my_cache.addOrder(order_6);
    // my_cache.addOrder(order_7);
    // my_cache.addOrder(order_8);

    // Order order_1 = Order("OrdId1", "SecId1", "Sell", 100, "User10", "Company2");
    // Order order_2 = Order("OrdId2", "SecId3", "Sell", 200, "User8", "Company2");
    // Order order_3 = Order("OrdId3", "SecId1", "Buy", 300, "User13", "Company2");
    // Order order_4 = Order("OrdId4", "SecId2", "Sell", 400, "User12", "Company2");
    // Order order_5 = Order("OrdId5", "SecId3", "Sell", 500, "User7", "Company2");
    // Order order_6 = Order("OrdId6", "SecId3", "Buy", 600, "User3", "Company1");
    // Order order_7 = Order("OrdId7", "SecId1", "Sell", 700, "User10", "Company2");
    // Order order_8 = Order("OrdId8", "SecId1", "Sell", 800, "User2", "Company1");
    // Order order_9 = Order("OrdId9", "SecId2", "Buy", 900, "User6", "Company2");
    // Order order_10 = Order("OrdId10", "SecId2", "Sell", 1000, "User5", "Company1");
    // Order order_11 = Order("OrdId11", "SecId1", "Sell", 1100, "User13", "Company2");
    // Order order_12 = Order("OrdId12", "SecId2", "Buy", 1200, "User9", "Company2");
    // Order order_13 = Order("OrdId13", "SecId1", "Sell", 1300, "User1", "Company2");

    // my_cache.addOrder(order_1);
    // my_cache.addOrder(order_2);
    // my_cache.addOrder(order_3);
    // my_cache.addOrder(order_4);
    // my_cache.addOrder(order_5);
    // my_cache.addOrder(order_6);
    // my_cache.addOrder(order_7);
    // my_cache.addOrder(order_8);
    // my_cache.addOrder(order_9);
    // my_cache.addOrder(order_10);
    // my_cache.addOrder(order_11);
    // my_cache.addOrder(order_12);
    // my_cache.addOrder(order_13);

    Order order_1 = Order("OrdId1", "SecId3", "Sell", 100, "User1", "Company1");
    Order order_2 = Order("OrdId2", "SecId3", "Sell", 200, "User3", "Company2");
    Order order_3 = Order("OrdId3", "SecId1", "Buy", 300, "User2", "Company1");
    Order order_4 = Order("OrdId4", "SecId3", "Sell", 400, "User5", "Company2");
    Order order_5 = Order("OrdId5", "SecId2", "Sell", 500, "User2", "Company1");
    Order order_6 = Order("OrdId6", "SecId2", "Buy", 600, "User3", "Company2");
    Order order_7 = Order("OrdId7", "SecId2", "Sell", 700, "User1", "Company1");
    Order order_8 = Order("OrdId8", "SecId1", "Sell", 800, "User2", "Company1");
    Order order_9 = Order("OrdId9", "SecId1", "Buy", 900, "User5", "Company2");
    Order order_10 = Order("OrdId10", "SecId1", "Sell", 1000, "User1", "Company1");
    Order order_11 = Order("OrdId11", "SecId2", "Sell", 1100, "User6", "Company2");

    my_cache.addOrder(order_1);
    my_cache.addOrder(order_2);
    my_cache.addOrder(order_3);
    my_cache.addOrder(order_4);
    my_cache.addOrder(order_5);
    my_cache.addOrder(order_6);
    my_cache.addOrder(order_7);
    my_cache.addOrder(order_8);
    my_cache.addOrder(order_9);
    my_cache.addOrder(order_10);
    my_cache.addOrder(order_11);


    // Get all orderds
    // auto res = my_cache.getAllOrders();
    // for(Order order : res)
    // {
    //     std::cout << order.orderId() << std::endl;
    // }

    // my_cache.printUserMap();
    // my_cache.printSecurityMap();
    // std::cout << my_cache.getMatchingSizeForSecurity(order_1.securityId()) << std::endl;

    // Cancel order
    // std::cout << "---------------" << std::endl;
    // my_cache.cancelOrder(order_4.orderId());
    // my_cache.printOrders();

    // Cancel orders for user
    // std::cout << "---------------" << std::endl;
    // my_cache.cancelOrdersForUser(order_2.user());
    // my_cache.printOrders();
    // my_cache.printUserMap();

    // Cancel orders for securityId with MinimumQty
    // std::cout << "---------------" << std::endl;
    // my_cache.cancelOrdersForSecIdWithMinimumQty(order_2.securityId(), 100);
    // my_cache.printOrders();
    // my_cache.printSecurityMap();


    std::cout << "End of program" << std::endl;
    return 0;
}

