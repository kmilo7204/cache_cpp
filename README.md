# Problem description
Implement an in-memory cache of order objects that supports adding new orders, removing existing orders and matching buy and sell orders.

  - **Order:** An "order" is a request to buy or sell a financial security (eg. bond, stock, commodity, etc.).
    - Each order is uniquely identified by an order id.
    - Each security has a different security id.
  - **Matching:** Order matching occurs for orders with the same security id, different side (buy or sell), and different company (company of person who requested the order).

## Task
Provide an implementation for the OrderCacheInterface class in OrderCacheInterface.h

For this an Order class is provided for you:
  - This class holds basic order information.
  - Do not remove the provided member variables and methods in the Order class.
  - You may add additional members if you like.

For your implementation of OrderCacheInterface:
  - Write a class that derives OrderCacheInterface.
  - Choose appropriate data structure(s) to hold Order objects and any additional data you'd like.
  - Implement the following methods (do not change their signatures):
    - addOrder()
    - cancelOrder()
    - cancelOrdersForUser()
    - cancelOrdersForSecIdWithMinimumQty()
    - getMatchingSizeForSecurity()
    - getAllOrders()
  - Feel free to add any additional methods and variables you'd like to your class.

## Examples
Find examples for the Order matching rules for getMatchingSizeForSecurity():
  - Your implementation of getMatchingSizeForSecurity() should give the total qty that can match for a security id.
  - Can only match orders with the same security id.
  - Can only match a Buy order with a Sell order.
  - Buy order can match against multiple Sell orders (and vice versa).
  - Any order quantity already allocated to a match cannot be reused as a match against a different order (eg the qty 3000 matched in the **Base case**)
  - Some orders may not match entirely or at all.
  - Users in the same company cannot match against each other.

### Base case
The Security ID "ABCD" has:
  - Buy  order with qty 10000
  - Sell order with qty  2000
  - Sell order with qty  1000

In this case Security Id "ABCD" has a total match of 3000. The Buy order's qty is big enough to match against both Sell orders and still has 7000 remaining.

### Example 1
Order matching example and explanation. The following set of orders will be added using addOrder()

| OrderID      | SecurityId | Side | Amount | User | Company |
| :-----------: | :-----------: | :-----------: | :-----------: | :-----------: | :-----------: |
|OrdId1| SecId1| Buy|  1000| User1| CompanyA|
|OrdId2| SecId2| Sell| 3000| User2| CompanyB|
|OrdId3| SecId1| Sell|  500| User3| CompanyA|
|OrdId4| SecId2| Buy|   600| User4| CompanyC|
|OrdId5| SecId2| Buy|   100| User5| CompanyB|
|OrdId6| SecId3| Buy|  1000| User6| CompanyD|
|OrdId7| SecId2| Buy|  2000| User7| CompanyE|
|OrdId8| SecId2| Sell| 5000| User8| CompanyE|

Explanation:
  - **SecId1**
      - `SecId1` has 1 Buy order and 1 Sell order.
      - Both orders are for users in CompanyA so they are not allowed to match.
      - There are no matches for `SecId1`.
  - **SecId2**
      - `OrdId2` matches quantity  600 against `OrdId4`.
      - `OrdId2` matches quantity 2000 against `OrdId7`.
      - `OrdId2` has a total matched quantity of 2600
      - `OrdId8` matches quantity 100 against `OrdId5` only.
          - `OrdId8` has a remaining qty of 4900.
      - `OrdId4` had its quantity fully allocated to match `OrdId2`.
          - No remaining qty on `OrdId4` for the remaining 4900 of `OrdId8`.
      - Total quantity matched for `SecId2` is 2700: 2600 + 100.
      - Note: there are other combinations of matches among the orders which
        would lead to the same result of 2700 total qty matching
  - **SecId3**
    - Has only one Buy order, no other orders to match against

### Example 2
Orders in cache:
| OrderID      | SecurityId | Side | Amount | User | Company |
| :-----------: | :-----------: | :-----------: | :-----------: | :-----------: | :-----------: |
|OrdId1| SecId1| Sell| 100| User10| Company2|
|OrdId2| SecId3| Sell| 200| User8| Company2|
|OrdId3| SecId1| Buy| 300| User13| Company2|
|OrdId4| SecId2| Sell| 400| User12| Company2|
|OrdId5| SecId3| Sell| 500| User7| Company2|
|OrdId6| SecId3| Buy| 600| User3| Company1|
|OrdId7| SecId1| Sell| 700| User10| Company2|
|OrdId8| SecId1| Sell| 800| User2| Company1|
|OrdId9| SecId2| Buy| 900| User6| Company2|
|OrdId10| SecId2| Sell| 1000| User5| Company1|
|OrdId11| SecId1| Sell| 1100| User13| Company2|
|OrdId12| SecId2| Buy| 1200| User9| Company2|
|OrdId13| SecId1| Sell| 1300| User1| Company2|

Total qty matching for security ids:
  - **SecId1:** 300
  - **SecId2:** 1000
  - **SecId3:** 600

### Example 2:
Orders in cache:
| OrderID      | SecurityId | Side | Amount | User | Company |
| :-----------: | :-----------: | :-----------: | :-----------: | :-----------: | :-----------: |
|OrdId1| SecId3| Sell| 100| User1| Company1|
|OrdId2| SecId3| Sell| 200| User3| Company2|
|OrdId3| SecId1| Buy| 300| User2| Company1|
|OrdId4| SecId3| Sell| 400| User5| Company2|
|OrdId5| SecId2| Sell| 500| User2| Company1|
|OrdId6| SecId2| Buy| 600| User3| Company2|
|OrdId7| SecId2| Sell| 700| User1| Company1|
|OrdId8| SecId1| Sell| 800| User2| Company1|
|OrdId9| SecId1| Buy| 900| User5| Company2|
|OrdId10| SecId1| Sell| 1000| User1| Company1|
|OrdId11| SecId2| Sell| 1100| User6| Company2|

Total qty matching for security ids:
  - **SecId1:** 900
  - **SecId2:** 600
  - **SecId3:** 0


