#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <condition_variable>

std::mutex mtx;
std::queue <std::string> messages;
std::condition_variable conVar;

void sender() {
	while (true) {
		std::string message;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		std::cout << "Enter a message (or 'exit' to quit): ";
		std::getline(std::cin, message);
	
		if (message == "exit") {
			{
				std::lock_guard<std::mutex> lock(mtx);
				messages.push("exit");
				conVar.notify_one();
			}
			break;
		}
		else {
			{
				std::lock_guard<std::mutex>lock(mtx);
				messages.push(message);
				std::cout << "Sent message: " << message << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				conVar.notify_one();
			}
			
		}
	}
}

void receiver() {
	while (true) {
		std::unique_lock<std::mutex>lock(mtx);
		conVar.wait(lock, [] {return !messages.empty(); });

		std::string message = messages.front();
		messages.pop();

		if (message == "exit") {
			std::cout << "Exiting receiver." << std::endl;
			break;
		}
		else {
			std::cout << "Received message:" << message << std::endl;
		}
	}
}

int main() {
	std::thread senderThread(sender);
	std::thread receiverThread(receiver);

	senderThread.join();
	receiverThread.join();

	return 0;
}