#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

enum class CollectionAction {
    add,
    remove,
    clear,
    assign
};

std::string to_string(CollectionAction action) {
    switch(action) {
        case CollectionAction::add: return "add";
        case CollectionAction::remove: return "remove";
        case CollectionAction::clear: return "clear";
        case CollectionAction::assign: return "assign";
    }
}

struct CollectionChangeNotification {
    CollectionAction action;
    std::vector<size_t> itemIndexes;
};

template <typename T>
struct HasNotifyChangeMethod {
private:
    template<typename U>
    static auto test(int) ->
        std::enable_if_t<
            std::is_same<
                decltype(
                    std::declval<U>().notifyChange(
                        CollectionChangeNotification{})),
                void>::value,
            std::true_type>;

    template<typename>
    static std::false_type test(...);
public:
    static constexpr bool value =
        std::is_same<decltype(test<T>(0)),std::true_type>::value;
};

template <
    typename T,
    typename CollectionObserver,
    class Allocator = std::allocator<T>,
    typename = std::enable_if_t<
        HasNotifyChangeMethod<CollectionObserver>::value, void>>
class ObservableVector final {
typedef typename std::vector<T, Allocator>::size_type size_type;

public:
    ObservableVector() noexcept(noexcept(Allocator()))
    : ObservableVector(Allocator()){}

    explicit ObservableVector(const Allocator& alloc) noexcept
    : data(alloc){}

    ObservableVector(
        size_type count, const T& value, const Allocator& alloc = Allocator())
    : data(count, value, alloc){}

    explicit ObservableVector(
        size_type count, const Allocator& alloc = Allocator())
    : data(count, alloc){}

    ObservableVector(ObservableVector&& other) noexcept
    : data(other.data){}

    ObservableVector(ObservableVector&& other, const Allocator& alloc)
    : data(other.data, alloc){}

    ObservableVector(
        std::initializer_list<T> init, const Allocator& alloc = Allocator())
    :data(init, alloc){}

    template<class InputIt>
    ObservableVector(
        InputIt first, InputIt last, const Allocator& alloc = Allocator())
    :data(first, last, alloc){}

    // copy assign operator
    ObservableVector& operator=(ObservableVector const & other) {
        if(this != &other) {
            data = other.data;

            for(auto o : observers) {
                if(o != nullptr) {
                    o->notifyChange({
                        CollectionAction::assign,
                        std::vector<size_t> {}
                    });
                }
            }
        }
        return *this;
    }

    ObservableVector& operator=(ObservableVector&& other)
    {
        if(this != &other) {
            data = std::move(other.data);
            for(auto o : observers) {
                if(o != nullptr) {
                    o->notifyChange({
                        CollectionAction::assign,
                        std::vector<size_t> {}
                    });
                }
            }
        }
        return *this;
    }

    void push_back(T&& value)
    {
        data.push_back(value);
        for(auto o : observers) {
            if(o != nullptr) {
                o->notifyChange({
                    CollectionAction::add,
                    std::vector<size_t> {data.size()-1}
                });
            }
        }
    }

    void pop_back()
    {
        data.pop_back();
        for(auto o : observers) {
            if(o != nullptr) {
                o->notifyChange({
                    CollectionAction::remove,
                    std::vector<size_t> {data.size()+1}
                });
            }
        }
    }

    void clear() noexcept
    {
        data.clear();
        for(auto o : observers) {
            if(o != nullptr) {
                o->notifyChange({
                    CollectionAction::clear,
                    std::vector<size_t> {}
                });
            }
        }
    }

    size_type size() const noexcept {
        return data.size();
    }

    [[nodiscard]] bool empty() const noexcept {
        return data.empty();
    }

    void add_observer(CollectionObserver * const o) {
        observers.push_back(o);
    }

    void remove_observer(CollectionObserver const * const o) {
        observers.erase(
            std::remove(
                std::begin(observers), std::end(observers), o),
            std::end(observers));
    }

private:
    std::vector<T, Allocator> data;
    std::vector<CollectionObserver*> observers;
};
