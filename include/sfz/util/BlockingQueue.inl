namespace sfz {

template<typename T>
void BlockingQueue<T>::push(const T& element)
{
	std::lock_guard<std::mutex> lock{mQueueMutex};
	mQueue.push(element);
	mQueueCond.notify_one();
} 

template<typename T>
T BlockingQueue<T>::pop()
{
	std::unique_lock<std::mutex> lock{mQueueMutex};
	mQueueCond.wait(lock, [this]() { return !mQueue.empty(); });
	T poppedObject{std::move(mQueue.front())};
	mQueue.pop();
	return std::move(poppedObject);
}

template<typename T>
std::unique_ptr<T> BlockingQueue<T>::tryPop()
{
	std::lock_guard<std::mutex> lock{mQueueMutex};
	if(mQueue.empty()) {
		return std::move(std::unique_ptr<T>{});
	}
	std::unique_ptr<T> ptr{new T{mQueue.front()}};
	mQueue.pop();
	return std::move(ptr);
}

template<typename T>
bool BlockingQueue<T>::empty() const
{
	std::lock_guard<std::mutex> lock{mQueueMutex};
	return mQueue.empty();
}

} // namespace sfz