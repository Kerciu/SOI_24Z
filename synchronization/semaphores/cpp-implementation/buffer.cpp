#include <iostream>
#include <vector>
#include <string>
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

	void print(std::string name)
	{
		std::cout << "\n### " << name << " " << values.size() << "[";
		for (auto v : values)
			std::cout << v << ",";
		std::cout << "]###\n";
	}

public:
	Buffer() : mutex(1), empty(0), full(bufferSize), semA(1), semB(1) { }

	void putA(char value)
	{
        full.p();
		mutex.p();
        if (size() < 5)
        {
		    values.push_back(value);
		    print("P");
        }
        mutex.v();
        empty.v();
	}

    void putB(char value)
	{
        full.p();
		mutex.p();
		values.push_back(value);
		if (size() > 3)
        {
		    values.push_back(value);
		    print("P");
        }
		mutex.v();
        empty.v();
	}


	char getA()
	{
        semA.p();
        empty.p();
		mutex.p();
		char v = values.front();
        values.erase(values.begin());
		mutex.v();
        full.v();
        semB.v();
		return v;
	}

    char getB()
	{
        semB.p();
        empty.p();
		mutex.p();
		char v = values.front();
        values.erase(values.begin());
		mutex.v();
        full.v();
        semA.v();
		return v;
	}

    size_t size()
    {
        return values.size();
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
		auto value = buffer.getA();
	}
	return NULL;
}

void* threadConsB(void* arg)
{
	for (int i = 0; i < 17; ++i)
	{
		auto value = buffer.getB();
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
	tid[3] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadConsA, 0, 0, &id);

	for (int i = 0; i < threadsCounts; i++)
		WaitForSingleObject(tid[i], INFINITE);
#else
	pthread_t tid[threadsCounts];

	pthread_create(&(tid[0]), NULL, threadProdA, NULL);
	pthread_create(&(tid[1]), NULL, threadProdB, NULL);
	pthread_create(&(tid[2]), NULL, threadConsA, NULL);
	pthread_create(&(tid[3]), NULL, threadConsA, NULL);

	for (int i = 0; i < threadsCounts; i++)
		pthread_join(tid[i], (void**)NULL);
#endif
	return 0;
}
