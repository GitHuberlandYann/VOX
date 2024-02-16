#ifndef BENCHMARK_HPP
# define BENCHMARK_HPP

# include <chrono> //https://www.youtube.com/watch?v=YG4jexlSAjc

class Bench
{
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> _startTimePoint;
		std::chrono::time_point<std::chrono::high_resolution_clock> _stampTimePoint;

	public:
		Bench()
		{
			_startTimePoint = std::chrono::high_resolution_clock::now();
			_stampTimePoint = _startTimePoint;
		}

		~Bench()
		{
		}

		void reset()
		{
			_startTimePoint = std::chrono::high_resolution_clock::now();
		}

		void stop( std::string msg )
		{
			auto endTimePoint = std::chrono::high_resolution_clock::now();

			auto start = std::chrono::time_point_cast<std::chrono::microseconds>(_startTimePoint).time_since_epoch().count();
			auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

			auto duration = end - start;
			double ms = duration * 0.001;

			std::cout << "Bench [\033[31m" << msg << "\033[0m] result: " << duration << "us | " << ms << "ms" << std::endl;
		}

		void stamp( std::string msg )
		{
			auto currentTimePoint = std::chrono::high_resolution_clock::now();

			if (msg != "NO") {
				auto start = std::chrono::time_point_cast<std::chrono::microseconds>(_stampTimePoint).time_since_epoch().count();
				auto end = std::chrono::time_point_cast<std::chrono::microseconds>(currentTimePoint).time_since_epoch().count();

				auto duration = end - start;
				double ms = duration * 0.001;

				std::cout << "\tstamp [" << msg << "] " << duration << "us | " << ms << "ms" << std::endl;
			}
			_stampTimePoint = currentTimePoint;
		}
};

#endif
