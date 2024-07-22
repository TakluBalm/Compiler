#ifndef STORE_H
#define STORE_H

#include <vector>
#include <string>
#include <iostream>

template<typename T>
class Store {
    static std::vector<T*> storage;

    public:
    template <class... Args>
    static T& newInstance(Args... args) {
        T* ref = new T(args...);
        Store::storage.push_back(ref);
        return *ref;
    }

    static void reset() {
        for(int i = 0; i < storage.size(); i++) {
            delete storage[i];
        }
        storage.clear();
    }
};

template<class T>
std::vector<T*> Store<T>::storage = std::vector<T*>();

#endif