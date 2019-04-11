#include "observable_vector.h"

class Observer {
public:
    void notifyChange(const CollectionChangeNotification& noti) {
        std::cout << "action: " << to_string(noti.action);
        if(!noti.itemIndexes.empty()) {
            std::cout << ", indexes: ";
            for(auto i : noti.itemIndexes)
                std::cout << i << ' ';
        }
        std::cout << std::endl;
    }
};

struct DummyType {
    CollectionAction action;
    std::vector<size_t> itemIndexes;
};

class InvalidObserver {
public:
    void notifyChange(const DummyType& noti) {
        std::cout << "action: " << to_string(noti.action);
        if(!noti.itemIndexes.empty()) {
            std::cout << ", indexes: ";
            for(auto i : noti.itemIndexes)
                std::cout << i << ' ';
        }
        std::cout << std::endl;
    }
};

int main() {
    ObservableVector<int, Observer> v;
    Observer o;

    // ObservableVector<int, InvalidObserver> iv;
    // InvalidObserver io;

    v.add_observer(&o);

    v.push_back(1);
    v.push_back(2);

    v.pop_back();

    v.clear();

    v.remove_observer(&o);

    v.push_back(3);
    v.push_back(4);

    v.add_observer(&o);

    ObservableVector<int, Observer> v2 {1,2,3};
    v = v2;

    v = ObservableVector<int, Observer> {7,8,9};
    return 0;
}
