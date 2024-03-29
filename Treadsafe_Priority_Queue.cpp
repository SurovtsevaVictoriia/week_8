#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

template < typename T >
class Threadsafe_Priority_Queue
{
public:

	Threadsafe_Priority_Queue() = default;

	Threadsafe_Priority_Queue(const Threadsafe_Priority_Queue& other)
	{
		std::lock_guard < std::mutex > lock(other.m_mutex);
		m_priority_queue = other.m_priority_queue;
	}

	Threadsafe_Priority_Queue& operator=(const Threadsafe_Priority_Queue& other)
	{
		std::scoped_lock lock(m_mutex, other.m_mutex);
		m_priority_queue = other.m_priority_queue;
	}

public:

	void push(T value)
	{
		std::lock_guard < std::mutex > lock(m_mutex);
		m_priority_queue.push(value);
		m_condition_variable.notify_one();
	}

	void wait_and_pop(T& value)
	{
		std::unique_lock < std::mutex > lock(m_mutex);

		m_condition_variable.wait(lock, [this] {return !m_priority_queue.empty(); });
		value = m_priority_queue.front();
		m_priority_queue.pop();
	}

	std::shared_ptr < T > wait_and_pop()
	{
		std::unique_lock < std::mutex > lock(m_mutex);

		m_condition_variable.wait(lock, [this] {return !m_priority_queue.empty(); });
		auto result = std::make_shared < T >(m_priority_queue.front());
		m_priority_queue.pop();

		return result;
	}

	bool try_pop(T& value)
	{
		std::lock_guard < std::mutex > lock(m_mutex);

		if (m_priority_queue.empty())
		{
			return false;
		}

		value = m_priority_queue.front();
		m_priority_queue.pop();

		return true;
	}

	std::shared_ptr < T > try_pop()
	{
		std::lock_guard < std::mutex > lock(m_mutex);

		if (m_priority_queue.empty())
		{
			return std::shared_ptr < T >();
		}

		auto result = std::make_shared < T >(m_priority_queue.front());
		m_priority_queue.pop();

		return result;
	}

	bool empty() const
	{
		std::lock_guard < std::mutex > lock(m_mutex);
		return m_priority_queue.empty();
	}

private:

	std::priority_queue < T >		m_priority_queue;
	std::condition_variable m_condition_variable;

private:

	mutable std::mutex m_mutex;
};

int main(int argc, char** argv)
{
	Threadsafe_Priority_Queue < int > queue;

	queue.push(42);

	auto ptr = queue.wait_and_pop();

	int value;

	bool result = queue.try_pop(value);

	system("pause");

	return EXIT_SUCCESS;
}
