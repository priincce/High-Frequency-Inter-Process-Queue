#include<iostream>
#include<vector>
#include<iomanip>
#include<algorithm>
#include<string>
class OrderBook{
public:
      enum class OrederType{Market, Limit, GoodTillCancelled, FillorKill_Limit};
      enum class Side{Buy, Sell};

      class Order{
      public:
            Order(int id, OrederType type, Side side, double price, int quantity):id(id), type(type), side(side), price(price), quantity(quantity){}
            int getId() const {return id;}
            OrederType getType()const{return type;}
            Side getSide()const {return side;}
            double getPrice()const {return price;}
            int getQuantity()const {return quantity;}
            void setQuantity(int new_quantity){ quantity= new_quantity;}

      private:
            int id;
            OrederType type;
            Side side;
            double price;
            int quantity;
      };
      void addOrder(const Order&order){
            orders.push_back(order);
      }
      void cancelOrder(int orderId){
            auto it=std::remove_if(orders.begin(), orders.end(), [orderId](const Order&order){
                  return order.getId()==orderId;
            });
            if(it != orders.end()){
                  std::cout<<"Cancelled Order ID : "<<orderId <<std::endl;
                  orders.erase(it, orders.end());
            }
      }
      void matchOrders(){
            for(auto it=orders.begin();it != orders.end(); ){
                  if(it->getType()==OrederType::Market){
                        auto matchIt = findMatch(it, it->getQuantity());
                        if(matchIt != orders.end()){
                              executeOrder(it, matchIt);
                              it = orders.erase(it);
                        }else{
                              ++it;
                        }
                  }else{
                        ++it;
                  }
            }


            for(auto it=orders.begin();it != orders.end(); ){
                  if(it->getType()==OrederType::GoodTillCancelled){
                        auto matchIt = findMatch(it, it->getQuantity());
                        if(matchIt != orders.end()){
                              executeOrder(it, matchIt);
                              it = orders.erase(it);
                        }else{
                              ++it;
                        }
                  }else{
                        ++it;
                  }
            }

            for(auto it=orders.begin();it != orders.end(); ){
                  if(it->getType()==OrederType::FillorKill_Limit){
                        auto matchIt = findMatch(it, it->getQuantity(), true);
                        if(matchIt != orders.end() && matchIt->getQuantity() >= it->getQuantity()){
                              executeOrder(it, matchIt);
                              it = orders.erase(it);
                        }else{
                              std::cout<<"Cancelled FOK Order ID: "<< it->getId() <<std::endl;
                              it = orders.erase(it);
                        }
                  }else{
                        ++it;
                  }
            }

            for(auto it=orders.begin();it != orders.end(); ){
                  if(it->getType()==OrederType::Limit){
                        auto matchIt = findMatch(it, it->getQuantity());
                        if(matchIt != orders.end()){
                              executeOrder(it, matchIt);
                              it = orders.erase(it);
                        }else{
                              ++it;
                        }
                  }else{
                        ++it;
                  }
            }
            
      }
      void printOrders()const {
            for(const auto& order: orders){
                  printOrder(order);
            }
      }

private:
      std::vector<Order>orders;
      std::vector<Order>::iterator findMatch(std::vector<Order>::iterator orderIt, int quantity, bool fullMatch=false){
            for(auto it = orders.begin(); it != orders.end(); ++it){
                  if(it->getSide() != orderIt->getSide() && ((orderIt->getSide() == Side::Buy && it->getPrice() <= orderIt->getPrice()) || (orderIt->getSide() == Side::Sell && it->getPrice() >= orderIt->getPrice())) && (!fullMatch || it->getQuantity() >= quantity)){
                        return it;
                  }
            }
            return orders.end();
      }
      void executeOrder(std::vector<Order>::iterator& orderIt, std::vector<Order>::iterator& matchIt){
            double fillPrice = matchIt->getPrice();
            std::cout<<"Matched Order ID: "<<orderIt->getId()<<" with Order ID: "<<matchIt->getId()<<" at Price: "<<std::fixed<<std::setprecision(10)<<matchIt->getPrice()<<std::endl;

            matchIt->setQuantity(matchIt->getQuantity()-orderIt->getQuantity());
            if(matchIt->getQuantity()==0){
                  orders.erase(matchIt);
            }
      }
      void printOrder(const Order& order)const {
            std::cout<< "Order ID: "<<order.getId()<<", Type: "<<static_cast<int>(order.getType())<<", Side: "<<(order.getSide()==Side::Sell?"Sell":"Buy") <<", Price: "<<order.getPrice()<<", Quantity: "<<order.getQuantity() << std::endl;
      }

};

int main(){
      OrderBook orderBook;

      OrderBook::Order order1(1, OrderBook::OrederType::Market, OrderBook::Side::Buy, 0, 10);
      OrderBook::Order order2(2, OrderBook::OrederType::Limit, OrderBook::Side::Sell, 101.0, 20);
      OrderBook::Order order3(3, OrderBook::OrederType::Limit, OrderBook::Side::Sell, 99.0, 5);
      OrderBook::Order order4(4, OrderBook::OrederType::Market, OrderBook::Side::Sell, 0, 15);
      OrderBook::Order order5(5, OrderBook::OrederType::GoodTillCancelled, OrderBook::Side::Buy, 102.0, 10);
      OrderBook::Order order6(6, OrderBook::OrederType::FillorKill_Limit, OrderBook::Side::Sell, 100.0, 8);
      OrderBook::Order order7(7, OrderBook::OrederType::FillorKill_Limit, OrderBook::Side::Buy, 99.0, 12);
      OrderBook::Order order8(8, OrderBook::OrederType::FillorKill_Limit, OrderBook::Side::Buy, 101.0, 8);


      orderBook.addOrder(order1);
      orderBook.addOrder(order2);
      orderBook.addOrder(order3);
      orderBook.addOrder(order4);
      orderBook.addOrder(order5);
      orderBook.addOrder(order6);
      orderBook.addOrder(order7);
      orderBook.addOrder(order8);

      std::cout<<"Order Book before matching "<<std::endl;
      orderBook.printOrders();

      orderBook.matchOrders();

      std::cout<<"Order Book after matching "<<std::endl;
      orderBook.printOrders();

      return 0;

}