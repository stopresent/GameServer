﻿#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <future>

int64 Calculate()
{
	int64 sum = 0;

	for (int32 i = 0; i < 100'000; i++)
		sum += i;

	return sum;
}


void PromiseWorker(std::promise<string>&& promise)
{
	promise.set_value("Secret Message");
}

void TaskWorker(std::packaged_task<int64(void)>&& task)
{
	task();
}

int main()
{

	// 동기 (snchronous) 실행
	int64 sum = Calculate();
	cout << sum << endl;

	// std::future
	{
		// 1) deferred -> lazy evaluation 지연해서 실행하세요
		// 2) async -> 별도의 쓰레드를 만들어서 실행하세요
		// 3) deferred | async -> 둘 중 알아서 골라주세요

		// 언젠가 미래에 결과물을 뱉어줄거야!
		std::future<int64> future = std::async(std::launch::async, Calculate);

		// TODO
		future.wait(); // 결과물이 이제서야 필요하다!

		std::future_status status = future.wait_for(1ms);
		if (status == future_status::ready)
		{

		}

		int64 sum = future.get(); // 결과물이 이제서야 필요하다!
	}




	// 멤버함수는 future을 바로 사용할수 없다

	class Knight
	{
	public:
		int64 GetHp() { return 100; }
	};

	Knight knight;
	std::future<int64> future2 = std::async(std::launch::async, &Knight::GetHp, knight); // knight.GetHp();



	// std::promise
	{
		// 미래(std::future)에 결과물을 반환해줄꺼라 약속(std::promise) 해줘~ (계약서?)
		std::promise<string> promise;
		std::future<string> future = promise.get_future();

		thread t(PromiseWorker, std::move(promise));

		string message = future.get(); // future.get을 하면 데이터가 사라지니 딱 한번만 사용해야 함
		cout << message << endl;

		t.join();

		// 타입에 맞는 데이터를 넣어주세요
	}




	// std::packaged_task
	{
		// Calculate를 다른 쓰레드에서 호출해주세요

		std::packaged_task<int64/*함수반환 타입*/(void/*매개변수 타입*/)> task(Calculate);
		std::future<int64> future = task.get_future();

		std::thread t(TaskWorker, std::move(task));

		int64 sum = future.get();
		cout << sum << endl;

		t.join();
	}





	// 결론)
	// mutex, condition_variable까지 가지 않고 단순한 애들을 처리할 수 있는
	// 딱 한번만 함수를 사용하고 더이상 사용하지 않을때?
	// 특히나, 한 번 발생하는 이벤트에 유용하다!
	// 닭잡는데 소잡는 칼을 쓸 필요 없다!
	// 1) async
	// 원하는 함수를 비동기적으로 실행
	// 2) promise
	// 결과물을 promise를 통해 future로 받아줌
	// 3) packaged_task
	// 원하는 함수의 실행 결과를 packaged_task를 통해 future로 받아줌

}