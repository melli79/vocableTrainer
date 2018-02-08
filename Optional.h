//
// Created by Melchior on 18/11/17.
//

#ifndef OPTIONAL_H
#define OPTIONAL_H

template <typename T>
class Optional {
public:
    virtual ~Optional() = default;
    virtual const T& get()= 0;
    virtual const T* orNull()= 0;
    virtual bool isPresent()= 0;
};

template <typename T>
class Present :public Optional<T> {
private:
    T value;
public:
    explicit Present(const T& value_) :value(value_) {}
    const T& get() override { return value; }
    const T* orNull() override { return &value; }
    bool isPresent() override { return true; }
};

template <typename T>
class Absent :public Optional<T> {
public:
    const T& get() override { return *(const T*) nullptr; }
    const T* orNull() override { return (const T*) nullptr; }
    bool isPresent() override { return false; }
};

#endif //OPTIONAL_H
