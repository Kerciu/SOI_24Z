#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include "monitor.h"

#define consSleep() std::this_thread::sleep_for(std::chrono::milliseconds(100))

#define threadIters 10
#define threadsCounts 4
#define bufferSize 9

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
	Buffer() : mutex(1), empty(0), full(bufferSize), stopA(0), stopB(0) { }

	inline bool canA() const
	{
		return values.size() < 5;
	}

	inline bool canB() const
	{
		return values.size() > 3;
	}

	void putA(char value)
	{
		full.p();
		mutex.p();

		if (!canA())
		{
			waitA = true;
			// printf("Producer A is waiting, leaving CS\n");
			mutex.v();
			// suspend process on dedicated semaphore
			stopA.p();
        	// printf("Producer A resumed\n");
			mutex.p();
			// printf("Producer A directly resumed, entered CS\n");
		}

		waitA = false;
		values.push_back(value);
		print("P_A");
		empty.v();

		if (canB() && waitB)
		{
			// printf("Resumed process B\n");
			stopB.v();
		}

		mutex.v();
	}

    void putB(char value)
	{
		full.p();
		mutex.p();

		if (!canB())
		{
			waitB = true;
			// printf("Producer B is waiting, leaving CS\n");
			mutex.v();
			// suspend process on dedicated semaphore
			stopB.p();
        	// printf("Producer B resumed\n");
			mutex.p();
			// printf("Producer B directly resumed, entered CS\n");
		}

		waitB = false;
		values.push_back(value);
		print("P_B");
		empty.v();

		if (canA() && waitA)
		{
			// printf("Resumed process A\n");
			stopA.v();
		}

		mutex.v();
	}

	// TODO: Deadlock after second consumer consumes

	char get(std::string who)
	{
		empty.p();
		mutex.p();

		char v = values.front();
		values.erase(values.begin());
		print("C_" + who);

		if (canA() && waitA) {
			// printf("Resumed process A\n");
			stopA.v();
		}
		if (canB() && waitB) {
			// printf("Resumed process B\n");
			stopB.v();
		}
		//else {
			mutex.v();
		//}

		full.v();

		return v;
	}
};

Buffer buffer;

void* threadProdA(void* arg)
{
	for (int i = 0; i < threadIters; ++i)
	{
        buffer.putA('A');
		// printf("Prod A Iters: %d\n", i + 1);
	}
	return NULL;
}

void* threadProdB(void* arg)
{
	for (int i = 0; i < threadIters; ++i)
	{
        buffer.putB('B');
		// printf("Prod B Iters: %d\n", i + 1);
	}
	return NULL;
}

void* threadConsA(void* arg)
{
	for (int i = 0; i < threadIters; ++i)
	{
		consSleep();
		auto value = buffer.get("A");
		// printf("Cons A Iters: %d\n", i+ 1);
	}
	return NULL;
}

void* threadConsB(void* arg)
{
	for (int i = 0; i < threadIters; ++i)
	{
		consSleep();
		auto value = buffer.get("B");
		// printf("Cons B Iters: %d\n", i + 1);
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
