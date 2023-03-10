#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <Windows.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <future>

#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"

LockQueue<int32> q;
LockStack<int32> s;

void Push()
{
	while (true)
	{
		int32 data = rand() % 100;
		q.Push(data);

		this_thread::sleep_for(10ms);
	}
}

int32 Pop()
{
	while (true)
	{
		int data = 0;
		if (q.TryPop(OUT data))
			cout << data << endl;
	}
}

int main()
{
	thread t1(Push);
	thread t2(Pop);
	thread t3(Pop);

	t1.join();
	t2.join();
	t3.join();

}