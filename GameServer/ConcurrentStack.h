#pragma once

#include <mutex>

template<typename T>
class LockStack
{
public:
	LockStack() {}

	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack&) = delete;

	void Push(T value)
	{
		lock_guard<mutex> lock<_mutex>;
		_stack.push(std::move(value));
		_conVar.notify_one();
	}

	bool TryPop(T& value)
	{
		lock_guard<mutex> lock<_mutex>;
		if (_stack.empty())
			return false;

		value = std::move(_stack.top());
		_stack.pop();
		return true;
	}

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock<_mutex>;
		_conVar.wait(lock, [this] {return _stack.empty() == false; }); // 스택에 데이터가 있을 때까지 대기
		value = std::move(_stack.top());
		_stack.pop();

	}

private:
	stack<T> _stack;
	mutex _mutex;
	condition_variable _conVar;
};

