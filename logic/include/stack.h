#ifndef STACK_H
#define STACK_H

#include "gmath.h"

namespace logic {
    class Stack {
    private:
        int topIndex;
        static const int MAX_SIZE = 50;
        gmath::GridEntry stackArray[MAX_SIZE];

    public:
        Stack();
        bool push(const gmath::GridEntry& entry);
        void pop();
        const gmath::GridEntry& top() const;
        int size() const;
        int capacity() const;
        bool empty() const;
        void clear();
        gmath::GridEntry get(int index) const;
    };
}

#endif  // STACK_H
