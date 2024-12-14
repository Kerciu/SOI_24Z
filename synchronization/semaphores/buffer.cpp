#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include "monitor.h"

int const threadsCounts = 4;

int const bufferSize = 9;


class Buffer
{
private:

	std::vector<char> values;
	Semaphore mutex;
    Semaphore empty, full;
	Semaphore stopA, stopB;
	bool waitA = false;
	bool waitB = false;

	void print(std::string name)
	{
		std::cout << "\n### " << name << " " << values.size() << "[";
		for (auto v : values)
			std::cout << v << ",";
		std::cout << "]###\n";
	}

public:
	Buffer() : mutex(1), empty(0), full(bufferSize), stopA(1), stopB(1) { }

	inline bool canA() const
	{
		return values.size() < 5 && values.size() <= bufferSize;
	}

	inline bool canB() const
	{
		return values.size() > 3 && values.size() <= bufferSize;
	}

	inline bool canConsume() const
	{
		return !values.empty();
	}

	void putA(char value)
	{
		full.p();
		mutex.p();

		if (!canA())
		{
			waitA = true;
			mutex.v();
			// suspend process on dedicated semaphore
			stopA.p();
			mutex.p();
			waitA = false;
		}

		values.push_back(value);
		print("P_A");
		empty.v();

		mutex.v();

		// if (canB() && waitB)
		// {
		// 	full.p();
		// 	stopB.v();
		// }
		// else
		// {
		// 	empty.v();
		// 	mutex.v();
		// }
	}

    void putB(char value)
	{
		full.p();
		mutex.p();

		if (!canB())
		{
			waitB = true;
			mutex.v();
			// suspend process on dedicated semaphore
			stopB.p();
			mutex.p();
			waitB = false;
		}

		values.push_back(value);
		print("P_B");
		empty.v();

		mutex.v();

		// if (canA() && waitA)
		// {
		// 	full.p();
		// 	stopA.v();
		// }
		// else
		// 	mutex.v();
	}

	// TODO: Deadlock after second consumer consumes

	char get(std::string who)
	{
		empty.p();
		mutex.p();

		char v;
		v = values.front();
		values.erase(values.begin());
		print("C_" + who);

		if (canConsume())
		{
			values.erase(values.begin());
			print("C_" + who);

			if (canA() && waitA) {
				stopA.v();
			}
			else {
				if (canB() && waitB) {
					stopB.v();
				}
				else
					mutex.v();
			}
		} else {
			if (canA() && waitA) {
				stopA.v();
			}
			else {
				if (canB() && waitB) {
					stopB.v();
				}
				else
					mutex.v();
			}
		}

		full.v();

		return v;
	}
};

Buffer buffer;

void* threadProdA(void* arg)
{
	for (int i = 0; i < 10; ++i)
	{
        buffer.putA('A');
	}
	return NULL;
}

void* threadProdB(void* arg)
{
	for (int i = 0; i < 10; ++i)
	{
        buffer.putB('B');
	}
	return NULL;
}

void* threadConsA(void* arg)
{
	for (int i = 0; i < 17; ++i)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		auto value = buffer.get("A");
	}
	return NULL;
}

void* threadConsB(void* arg)
{
	for (int i = 0; i < 17; ++i)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		auto value = buffer.get("B");
	}
	return NULL;
}

int main()
{
#ifdef _WIN32
	HANDLE tid[threadsCounts];
	DWORD id;

	tid[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProdA, 0, 0, &id);
	tid[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProdB, 0, 0, &id);
	tid[2] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadConsA, 0, 0, &id);
	tid[3] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadConsB, 0, 0, &id);

	for (int i = 0; i < threadsCounts; i++)
		WaitForSingleObject(tid[i], INFINITE);
#else
	pthread_t tid[threadsCounts];

	pthread_create(&(tid[0]), NULL, threadProdA, NULL);
	pthread_create(&(tid[1]), NULL, threadProdB, NULL);
	pthread_create(&(tid[2]), NULL, threadConsA, NULL);
	pthread_create(&(tid[3]), NULL, threadConsB, NULL);

	for (int i = 0; i < threadsCounts; i++)
		pthread_join(tid[i], (void**)NULL);
#endif
	return 0;
}
