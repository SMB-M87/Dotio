#include "stack.h"

namespace logic {
    Stack::Stack() : topIndex(-1) {}

    bool Stack::push(const gmath::GridEntry& entry) {
        if (topIndex < MAX_SIZE - 1)
        {
            stackArray[++topIndex] = entry;
            return true;
        }
        return false;
    }

    void Stack::pop() {
        if (!empty())
            topIndex--;
    }

    const gmath::GridEntry& Stack::top() const {
        if (!empty())
            return stackArray[topIndex];
    }

    int Stack::size() const { return topIndex + 1; }

    int Stack::capacity() const { return MAX_SIZE; }

    bool Stack::empty() const { return topIndex == -1; }

    void Stack::clear() { topIndex = -1; }

    gmath::GridEntry Stack::get(int index) const { 
        if (index >= 0 && index <= topIndex)
            return stackArray[index];
    }
}  // namespace logic
