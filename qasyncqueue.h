#ifndef QASYNCQUEUE_H
#define QASYNCQUEUE_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QQueue>

template<class T> class QAsyncQueue
{
QQueue<T> queue_;
QMutex mutex_;
int max_;

public:
QAsyncQueue(uint max = -1)
    : max_{max}
{
}

~QAsyncQueue()
{
clean();
}

uint count()
{
mutex_.lock();
int count = queue_.count();
mutex_.unlock();
return count;
}

bool isFull()
{
if (-1 == max_)
return false;

mutex_.lock();
int count = queue_.count();
mutex_.unlock();
return count >= max_;
}

bool isEmpty()
{
mutex_.lock();
bool empty = queue_.isEmpty();
mutex_.unlock();
return empty;
}

void clean()
{
mutex_.lock();
queue_.clear();
mutex_.unlock();
}
template<typename Iterator>
void pushMany(Iterator bg, Iterator en)
{
    mutex_.lock();
    while(bg<en)
        queue_.enqueue(*(bg++));
    mutex_.unlock();
}

void push(const T& t)
{
mutex_.lock();
queue_.enqueue(t);
mutex_.unlock();
}

T pull()
{
mutex_.lock();
T i = queue_.dequeue();
mutex_.unlock();
return i;
}

};

#endif // QASYNCQUEUE_H
