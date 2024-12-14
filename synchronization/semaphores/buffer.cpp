#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include "monitor.h"

#define consSleep() std::this_thread::sleep_for(std::chrono::milliseconds(100))

constexpr int THREADS_ITERS = 10;
constexpr int THREADS_COUNTS = 4;
constexpr int BUFFER_SIZE = 9;

class Buffer
{
private:

	std::vector<char> values;
	Semaphore mutex;
    Semaphore empty, full;
	Semaphore stopA, stopB;
	bool waitA = false;
	bool waitB = false;

	void print(std::string name) const noexcept
	{
		std::cout << "\n### " << name << " " << values.size() << "[";
		for (auto v : values)
			std::cout << v << ",";
		std::cout << "]###\n";
	}

public:
	Buffer() : mutex(1), empty(0), full(BUFFER_SIZE), stopA(0), stopB(0) { }

	inline bool canA() const noexcept
	{
		return values.size() < 5;
	}

	inline bool canB() const noexcept
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

void* threadProdA(void* arg) noexcept
{
	for (int i = 0; i < THREADS_ITERS; ++i)
	{
        buffer.putA('A');
		// printf("Prod A Iters: %d\n", i + 1);
	}
	return nullptr;
}

void* threadProdB(void* arg) noexcept
{
	for (int i = 0; i < THREADS_ITERS; ++i)
	{
        buffer.putB('B');
		// printf("Prod B Iters: %d\n", i + 1);
	}
	return nullptr;
}

void* threadConsA(void* arg) noexcept
{
	for (int i = 0; i < THREADS_ITERS; ++i)
	{
		consSleep();
		auto value = buffer.get("A");
		// printf("Cons A Iters: %d\n", i+ 1);
	}
	return nullptr;
}

void* threadConsB(void* arg) noexcept
{
	for (int i = 0; i < THREADS_ITERS; ++i)
	{
		consSleep();
		auto value = buffer.get("B");
		// printf("Cons B Iters: %d\n", i + 1);
	}
	return nullptr;
}

int main()
{
#ifdef _WIN32
	HANDLE tid[THREADS_COUNTS];
	DWORD id;

	tid[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProdA, 0, 0, &id);
	tid[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProdB, 0, 0, &id);
	tid[2] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadConsA, 0, 0, &id);
	tid[3] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadConsB, 0, 0, &id);

	for (int i = 0; i < THREADS_COUNTS; i++)
		WaitForSingleObject(tid[i], INFINITE);
#else
	pthread_t tid[THREADS_COUNTS];

	pthread_create(&(tid[0]), NULL, threadProdA, NULL);
	pthread_create(&(tid[1]), NULL, threadProdB, NULL);
	pthread_create(&(tid[2]), NULL, threadConsA, NULL);
	pthread_create(&(tid[3]), NULL, threadConsB, NULL);

	for (int i = 0; i < THREADS_COUNTS; i++)
		pthread_join(tid[i], (void**)NULL);
#endif
	return 0;
}
