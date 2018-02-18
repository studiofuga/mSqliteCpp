//
// Created by Federico Fuga on 18/02/18.
//

#ifndef SQLITE_MAKE_UNIQUE_H
#define SQLITE_MAKE_UNIQUE_H

#include <memory>

namespace sqlite {
    namespace utils {

        template<typename T, typename ... Ts>
        std::unique_ptr<T> make_unique(Ts &&... params) {
            return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
        };

    } // ns utils
} // ns sqlite

#endif //SQLITE_MAKE_UNIQUE_H
