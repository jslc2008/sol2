// The MIT License (MIT)

// Copyright (c) 2013-2016 Rapptz and contributors

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SOL_PROXY_HPP
#define SOL_PROXY_HPP

#include "traits.hpp"
#include "object.hpp"
#include "function.hpp"
#include "proxy_base.hpp"

namespace sol {
template<typename Table, typename Key>
struct proxy : public proxy_base<proxy<Table, Key>> {
private:
    Table tbl;
    If<std::is_array<Unqualified<Key>>, Key&, Unqualified<Key>> key;

public:

    template<typename T>
    proxy(Table table, T&& key) : tbl(table), key(std::forward<T>(key)) {}

    template<typename T>
    proxy& set(T&& item) {
        tbl.set(key, std::forward<T>(item));
        return *this;
    }

    template<typename... Args>
    proxy& set_function(Args&&... args) {
        tbl.set_function(key, std::forward<Args>(args)...);
        return *this;
    }

    template<typename U, EnableIf<Function<Unqualified<U>>> = 0>
    proxy& operator=(U&& other) {
        tbl.set_function(key, std::forward<U>(other));
        return *this;
    }

    template<typename U, DisableIf<Function<Unqualified<U>>> = 0>
    proxy& operator=(U&& other) {
        tbl.set(key, std::forward<U>(other));
        return *this;
    }

    template<typename T>
    decltype(auto) get() const {
        return tbl.template get<T>( key );
    }

    template <typename K>
    decltype(auto) operator[](K&& key) const {
        return get<table>()[std::forward<K>(key)];
    }

    template<typename... Ret, typename... Args>
    decltype(auto) call(Args&&... args) {
        return get<function>().call<Ret...>(std::forward<Args>(args)...);
    }

    template<typename... Args>
    decltype(auto) operator()(Args&&... args) {
        return call<>(std::forward<Args>(args)...);
    }
};

template<typename Table, typename Key, typename T>
inline bool operator==(T&& left, const proxy<Table, Key>& right) {
    return left == right.template get<std::decay_t<T>>();
}

template<typename Table, typename Key, typename T>
inline bool operator==(const proxy<Table, Key>& right, T&& left) {
    return right.template get<std::decay_t<T>>() == left;
}

template<typename Table, typename Key, typename T>
inline bool operator!=(T&& left, const proxy<Table, Key>& right) {
    return right.template get<std::decay_t<T>>() != left;
}

template<typename Table, typename Key, typename T>
inline bool operator!=(const proxy<Table, Key>& right, T&& left) {
    return right.template get<std::decay_t<T>>() != left;
}
} // sol

#endif // SOL_PROXY_HPP
