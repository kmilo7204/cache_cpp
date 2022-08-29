// https://www.geeksforgeeks.org/design-a-data-structure-for-lru-cache/
#include "OrderCache.h"
#include "Node.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <vector>



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
            std::cout << "SecurityId is not in the cache" << std::endl;
            return 0;
        }

        auto orderIds = mapSecurity[securityId];
        // std::unordered_map<std::string, bool> buys = getBuyOrders(orderIds);

        // Get all the orders that want to buy something
        std::unordered_set<std::string> buyOrders = getBuyOrdersIds(orderIds);
        // std::cout << buyOrders.size() << std::endl;
        // for(auto buy : buyOrders)
        // {
        //     std::cout << buy << std::endl;
        // }

        // Get all the orders that want to seel something
        std::vector<Order> sellOrders = getSellOrders(orderIds);
        // std::cout << sellOrders.size() << std::endl;
        // for(auto sell : sellOrders)
        // {
        //     std::cout << sell.orderId() << std::endl;
        // }

        unsigned int matchSize = 0;
        for (auto sellOrder : sellOrders)
        {
        //     // buysOnly =
            getUnUsedBuys(sellOrder, buyOrders, matchSize);
        //     // int remaining = sell.qty();
        //     // for (auto buy: buys)
        //     // {
        //     //     if (buy.second)
        //     //     {
        //     //         continue;
        //     //     }
        //     //     Order* buyOrder = mapOrderId[buy.first]->order;
        //     //     if ((sell.company() == buyOrder->company()) || remaining < buyOrder->qty())
        //     //     {
        //     //         continue;
        //     //     }
        //     //     remaining -= buyOrder->qty();
        //     //     buys[buyOrder->orderId()] = true;
        //     // }
        }
        return matchSize;
        //return sumBuyOrdersOnly(buysOnly);
        // return sumBuyOrders(buys);
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
        // std::cout << "---------------" << std::endl;
    }

    // unsigned int sumBuyOrders(std::unordered_map<std::string, bool>& buys)
    // {
    //     int total = 0;
    //     for (auto buy: buys)
    //     {
    //             if (buy.second)
    //             {
    //                 total += mapOrderId[buy.first]->order->qty();
    //             }
    //     }
    //     return total;
    // }

    // std::unordered_map<std::string, bool> getBuyOrders(const std::unordered_set<std::string>& orderIds)
    // {

    //     std::unordered_map<std::string, bool> buys;
    //     for(auto orderId:orderIds)
    //     {
    //         Order* order = &mapOrderId[orderId]->order;
    //         if (order->side() == "Buy") {
    //             buys[orderId] = false;
    //         }
    //     }
    //     return buys;
    // }

    std::unordered_set<std::string> getBuyOrdersIds(const std::unordered_set<std::string>& orderIds)
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
        return buys;
    }

    std::vector<Order> getSellOrders(const std::unordered_set<std::string>& orderIds)
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
                    // std::cout << "Company is in the map: " << order->company() << std::endl;
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
        return sells;
    }
};

int main()
{
    // For now it is an order object, but I would need to make the double linked list here.
    // I could add the required elements inside that class so it might be easier.
    std::cout << "Starting" << std::endl;

    Order order_1 = Order("OrdId1", "SecId1", "Buy", 1000, "User1", "CompanyA");
    Order order_2 = Order("OrdId2", "SecId2", "Sell", 3000, "User2", "CompanyB");
    Order order_3 = Order("OrdId3", "SecId1", "Sell", 500, "User3", "CompanyA");
    Order order_4 = Order("OrdId4", "SecId2", "Buy", 600, "User4", "CompanyC");
    Order order_5 = Order("OrdId5", "SecId2", "Buy", 100, "User5", "CompanyB");
    Order order_6 = Order("OrdId6", "SecId3", "Buy", 1000, "User6", "CompanyD");
    Order order_7 = Order("OrdId7", "SecId2", "Buy", 2000, "User7", "CompanyE");
    Order order_8 = Order("OrdId8", "SecId2", "Sell", 5000, "User8", "CompanyE");

    Cache my_cache{};
    my_cache.addOrder(order_1);
    my_cache.addOrder(order_2);
    my_cache.addOrder(order_3);
    my_cache.addOrder(order_4);
    my_cache.addOrder(order_5);
    my_cache.addOrder(order_6);
    my_cache.addOrder(order_7);
    my_cache.addOrder(order_8);

    // Order order_1 = Order("OrdId1", "SecId1", "Buy", 1000, "User1", "CompanyA");
    // Order order_2 = Order("OrdId2", "SecId2", "Sell", 3000, "User2", "CompanyB");
    // Order order_3 = Order("OrdId3", "SecId1", "Sell", 500, "User3", "CompanyA");
    // Order order_4 = Order("OrdId4", "SecId2", "Buy", 600, "User4", "CompanyC");
    // Order order_5 = Order("OrdId5", "SecId2", "Buy", 100, "User5", "CompanyB");
    // Order order_6 = Order("OrdId6", "SecId3", "Buy", 1000, "User6", "CompanyD");
    // Order order_7 = Order("OrdId7", "SecId2", "Buy", 2000, "User7", "CompanyE");
    // Order order_8 = Order("OrdId8", "SecId2", "Sell", 5000, "User8", "CompanyE");


    // auto res = my_cache.getAllOrders();

    // // std::cout << res.size() << std::endl;

    // for(Order order : res)
    // {
    //     std::cout << order.orderId() << std::endl;
    // }

    // my_cache.printOrders();
    // my_cache.printUserMap();
    my_cache.printSecurityMap();
    std::cout << "---------------" << std::endl;
    std::cout << my_cache.getMatchingSizeForSecurity(order_2.securityId()) << std::endl;
    // // my_cache.cancelOrder(order_4.orderId());

    // std::cout << "---------------" << std::endl;
    // // my_cache.printUserMap();
    // // my_cache.printOrders();

    // std::cout << "---------------" << std::endl;
    // my_cache.cancelOrdersForUser(order_2.user());

    // std::cout << "---------------" << std::endl;
    // my_cache.printOrders();
    // my_cache.printUserMap();

    // my_cache.cancelOrdersForSecIdWithMinimumQty(order_2.securityId(), 100);
    // my_cache.printUserMap();
    // my_cache.printOrders();
    // my_cache.printSecurityMap();


    std::cout << "End of program" << std::endl;
    return 0;
}

