//
// Created by Priory on 3/9/2024.
//

#ifndef TRACKER_H
#define TRACKER_H

namespace logic {
    template<typename T>
    concept IsActive = requires(T t) {
        { t.IsActive } -> std::convertible_to<bool>;
    };

    template<typename T>
    struct Tracker {
    private:
        T* _instances{};
        size_t _size{};
        size_t _index{};
        T* _instance;
        explicit Tracker(T* instance) : _instance(instance) {}

    public:
        explicit Tracker(T* instances, const size_t size, const size_t index) :
            _instances(instances), _size(size), _index(index), _instance(&_instances[0]) {}

        T& operator[](const int index) { return GetInstances()[index % _size]; }

        void AddInstance(T instance)
            requires IsActive<T>
        {
            T& i = _instances[_index++] = instance;
            i.IsActive = true;

            if (_index == _size)
                _index = 0;
        }

        T* GetInstances() const { return _instances; }

        [[nodiscard]] size_t GetSize() const { return _size; }

        T& operator*() { return *_instance; }

        Tracker& operator++() {
            ++_instance;
            return *this;
        }

        bool operator!=(const Tracker& other) const { return _instance != other._instance; }

        Tracker begin() { return Tracker(&_instances[0]); }
        Tracker end() { return Tracker(&_instances[_size]); }

        [[nodiscard]] size_t GetCapacity() const { return _size; };
    };

    template<typename T, size_t size>
    struct TrackerData {
    private:
        size_t _capacity{ size };
        T _objects[size]{};
        size_t _index{};
        Tracker<T> _tracker;

    public:
        T& operator[](const int index) { return _tracker[index % size]; }

        void operator+=(T instance) { return _tracker.AddInstance(instance); }

        TrackerData() : _tracker(_objects, size, 0) {}

        [[nodiscard]] Tracker<T>& GetTracker() { return _tracker; }

        void AddInstance(T instance) { _tracker.AddInstance(instance); };

        [[nodiscard]] T* GetInstances() const { return _tracker.GetInstances(); }

        [[nodiscard]] size_t GetCapacity() const { return _tracker.GetCapacity(); }
    };
}  // namespace logic

#endif
