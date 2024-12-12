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
    Semaphore full, empty;
	Semaphore semA, semB;
	bool readA = false;
	bool readB = false;

	void print(std::string name)
	{
		std::cout << "\n### " << name << " " << values.size() << "[";
		for (auto v : values)
			std::cout << v << ",";
		std::cout << "]###\n";
	}

public:
	Buffer() : mutex(1), empty(0), full(bufferSize), semA(1), semB(1)
	{ }

	bool canA()
	{
		return values.size() < 5;
	}

	bool canB()
	{
		return values.size() > 3;
	}

	void putA(char value)
	{
		//semProdA.p();
		full.p();
		mutex.p();
		if (canA())
		{
			values.push_back(value);
			print("P_A");

			// if (canB()) {
			// 	semProdB.v();
			// }

			empty.v();
		} else {
			full.v();
		}

		mutex.v();
	}

    void putB(char value)
	{
        // semProdB.p();
		full.p();
		mutex.p();
		if (canB())
		{
			values.push_back(value);
			print("P_B");

			// if (canA()) {
			// 	semProdA.v();
			// }

			empty.v();
		} else {
			full.v();
		}

		mutex.v();
	}

	char getA()
	{
		semA.p();
		empty.p();
		mutex.p();
		char v = 'o';

		// if (!values.empty())
		// {
			v = values.front();
			readA = true;
			if (readB)
			{
				values.erase(values.begin());
				print("C_A");
				readA = readB = false;
				semA.v();
				semB.v();
				full.v();
			} else {
				empty.v();
			}
		// } else {
		// 	empty.v();
		// }

		// semB.v();
		mutex.v();

		return v;
	}

	char getB()
	{
		semB.p();
		empty.p();
		mutex.p();
		char v = 'o';

		// if (!values.empty())
		// {
			v = values.front();
			readB = true;
			if (readA)
			{
				values.erase(values.begin());
				print("C_B");
				readA = readB = false;
				semA.v();
				semB.v();
				full.v();
			} else {
				empty.v();
			}
		// } else {
		// 	empty.v();
		// }

		// semA.v();
		mutex.v();

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
	for (int i = 0; i < 10; ++i)
	{
		auto value = buffer.getA();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return NULL;
}

void* threadConsB(void* arg)
{
	for (int i = 0; i < 10; ++i)
	{
		auto value = buffer.getB();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
