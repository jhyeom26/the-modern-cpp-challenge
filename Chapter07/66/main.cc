//
// there are 3 desks where customers can be served at the same time,
// customers take a ticket with a service number from a ticketing machine,
// and wait until thier number is next for service at one of the desks.
// they are served in the order given by their ticket.
// Every time a service desk finishes serving a customer,
// the next customner in order is served.
// The simulation should be stop after a particular number of customers
// have been issued tickets and served.
//

#include <array>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <thread>
#include <vector>

#include <spdlog/spdlog.h>

class TicketingMachine{
public:
    //! @param[in] start start ticket number
    TicketingMachine(int const start)
    :firstTicket_(start), lastTicket_(start){}

    int next(){
        return ++lastTicket_;
    }

    int last() const{
        return lastTicket_ - 1;
    }

    void reset(){
        lastTicket_ = firstTicket_;
    }

private:
    int firstTicket_;
    int lastTicket_;
};

class Customer{
public:
    Customer(int const ticketNumber)
    : ticketNumber_(ticketNumber){}

    int getTicketNumber() const noexcept{
         return ticketNumber_;
    }

private:
    int ticketNumber_;
};

template<
    typename T,
    template<typename> typename Distribution = std::uniform_int_distribution,
    typename... DistParam>
T getRandomNumber(DistParam&&... args){
    std::mt19937 randomEngine;
    randomEngine.seed(std::random_device()());

    Distribution<T> dist(std::forward<DistParam>(args)...);
    return dist(randomEngine);
}

int main(){
    std::priority_queue<
        Customer,
        std::vector<Customer>,
        std::function<bool(Customer, Customer)>> customers(
            [](Customer const& lhs, Customer const& rhs) -> bool{
                // The lower ticket number is prior to the higher ticket number
                return lhs.getTicketNumber() > rhs.getTicketNumber();
            });
    bool officeOpen = true;
    std::mutex mtx;
    std::condition_variable cv;

    std::vector<std::thread> desks;
    for(int i = 0 ; i < 3; i++){
        desks.emplace_back(
            [i, &officeOpen, &mtx, &cv, &customers](){
                spdlog::info("desk {} open", i);
                while(!customers.empty() || officeOpen){
                    std::unique_lock<std::mutex> lk(mtx);

                    cv.wait_for(
                        lk, std::chrono::seconds(1),
                        [&customers](){ return !customers.empty(); });

                    // spurious wake up
                    if(customers.empty()){
                        continue;
                    }

                    auto const customer = customers.top();
                    customers.pop();

                    spdlog::info(
                        "[-] desk {} handling customer {}",
                        i, customer.getTicketNumber());

                    spdlog::info(
                        "[=] queue size: {}", customers.size());

                    lk.unlock();
                    cv.notify_one();

                    // Handling the job in 2-3 seconds.
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(
                            getRandomNumber<int>(2000, 3000)));

                    spdlog::info(
                        "[ ] desk {} done with customer {}",
                        i, customer.getTicketNumber());
                }
                spdlog::info("desk {} closed", i);
            });
    }

    std::thread store(
        [&officeOpen, &customers, &mtx, &cv](){
            TicketingMachine tm(100);
            for(int i = 1; i <= 25; i++){
                Customer customer(tm.next());

                {
                    std::scoped_lock<std::mutex> lk(mtx);
                    customers.push(customer);
                }

                spdlog::info(
                    "[+] new customer with ticket {}",
                    customer.getTicketNumber());
                spdlog::info("[=] queue size: {}", customers.size());

                cv.notify_one();

                // customers are visiting with an interval of 0.2-0.5 seconds.
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(
                        getRandomNumber<int>(20, 50)));
            }
            officeOpen = false;
        });

    store.join();

    for(auto& desk : desks){
        desk.join();
    }

    return 0;
}
