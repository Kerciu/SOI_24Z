#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include "../utils/monitor.h"

#define consSleep() std::this_thread::sleep_for(std::chrono::milliseconds(100))

constexpr int THREADS_ITERS = 10;
constexpr int THREADS_COUNTS = 4;
constexpr int BUFFER_SIZE = 9;

class Buffer : Monitor
{
private:

	std::vector<char> values;
    Condition A, B, C;

	void print(std::string name) const noexcept
	{
		std::cout << "\n### " << name << " " << values.size() << "[";
		for (auto v : values)
			std::cout << v << ",";
		std::cout << "]###\n";
	}

public:
	Buffer() {}

	inline bool canA() const noexcept
	{
		return values.size() < 5;
	}

	inline bool canB() const noexcept
	{
		return values.size() > 3 && values.size() < BUFFER_SIZE;
	}

    inline bool canCons() const noexcept
    {
        return !values.empty();
    }

	void putA(char value)
	{
		this->enter();

		if (!canA()) this->wait(A);

		values.push_back(value);
		print("P_A");

		if (canB())
            this->signal(B);
        
        if (canCons())
            this->signal(C);

		this->leave();
	}

    void putB(char value)
	{
		this->enter();

		if (!canB()) this->wait(B);

		values.push_back(value);
		print("P_B");

		if (canA())
            this->signal(A);
        
        if (canCons())
            this->signal(C);

		this->leave();
	}

	char get(std::string who)
	{
		this->enter();

        if (!canCons()) this->wait(C);

		char v = values.front();
		values.erase(values.begin());
		print("C_" + who);

		if (canA()) {
            this->signal(A);
        }
        if (canB()) {
            this->signal(B);
        }

        this->leave();
		return v;
	}
};

Buffer buffer;

void* threadProdA(void* arg) noexcept
{
	for (int i = 0; i < THREADS_ITERS; ++i)
	{
        buffer.putA('A');
	}
	return nullptr;
}

void* threadProdB(void* arg) noexcept
{
	for (int i = 0; i < THREADS_ITERS; ++i)
	{
        buffer.putB('B');
	}
	return nullptr;
}

void* threadConsA(void* arg) noexcept
{
	for (int i = 0; i < THREADS_ITERS; ++i)
	{
		consSleep();
		auto value = buffer.get("A");
	}
	return nullptr;
}

void* threadConsB(void* arg) noexcept
{
	for (int i = 0; i < THREADS_ITERS; ++i)
	{
		consSleep();
		auto value = buffer.get("B");
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
