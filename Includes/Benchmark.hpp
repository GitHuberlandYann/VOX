#ifndef BENCHMARK_HPP
# define BENCHMARK_HPP

# include <chrono> //https://www.youtube.com/watch?v=YG4jexlSAjc

class Bench
{
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> _startTimePoint;

	public:
		Bench()
		{
			_startTimePoint = std::chrono::high_resolution_clock::now();
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

			std::cout << "Bench [" << msg << "] result: " << duration << "us ( " << ms << "ms)" << std::endl;
		}
};

#endif
