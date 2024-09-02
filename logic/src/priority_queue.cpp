#include "priority_queue.h"

namespace logic {
    PriorityQueue::PriorityQueue(const gmath::GridEntry& g) : length(0), goal(g) {}

    void PriorityQueue::push(logic::Vertex& vertex) {
        if (length >= MAX_CAPACITY)
            return;

        int hole = ++length;
        array[hole] = &vertex;

        while (hole > 1 && array[hole / 2]->Calculate(goal) > vertex.Calculate(goal)) {
            array[hole] = array[hole / 2];
            hole /= 2;
        }
        array[hole] = &vertex;
    }

    logic::Vertex& PriorityQueue::pop() {
        if (length == 0) {
            return logic::Vertex::NullVertex;
        } else {
            Vertex& min = *array[1];
            array[1] = array[length--];
            percolate(1);
            return min;
        }
    }

    void PriorityQueue::percolate(int hole) {
        int child;
        Vertex* tmp = array[hole];

        for (; hole * 2 <= length; hole = child) {
            child = hole * 2;
            if (child != length && array[child + 1]->Calculate(goal) < array[child]->Calculate(goal)) {
                child++;
            }
            if (array[child]->Calculate(goal) < tmp->Calculate(goal)) {
                array[hole] = array[child];
            } else {
                break;
            }
        }
        array[hole] = tmp;
    }

    bool PriorityQueue::empty() const { return length == 0; }

    int PriorityQueue::size() const { return length; }

    void PriorityQueue::clear() { length = 0; }
}