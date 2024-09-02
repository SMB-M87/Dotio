#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "vertex.h"

namespace logic {
    const int MAX_CAPACITY = 250;

    class PriorityQueue {
    public:
        PriorityQueue(const gmath::GridEntry& goal);

        void push(logic::Vertex& vertex);
        logic::Vertex& pop();
        bool empty() const;
        int size() const;
        void clear();

    private:
        logic::Vertex* array[MAX_CAPACITY];
        gmath::GridEntry goal;
        int length;

        void percolate(int hole);
    };
}

#endif