#pragma once

#include <stack>
#include <vector>

namespace ECS
{
	/// <summary>
	/// A Stack using an std::vector&lt;T&gt; with a GetContainer() function to access the vector
	/// </summary>
	/// <typeparam name="T">The data type to store in the vector</typeparam>
	template<typename T> class IdPoolStackAdapter : public std::stack<T, std::vector<T>>
	{
	public:
		/// <summary>
		/// Gets the underlying container
		/// </summary>
		/// <returns>The container the stack has been built on top of</returns>
		std::vector<T>& GetContainer() { return this->c; }
	};

	/// <summary>
	/// A pool of IDs that can be taken out and be put back into the pool.
	/// </summary>
	/// <typeparam name="T">The data type for the IDs (has to implement T+T, T-T, (T)0, (T)1 and T==T)</typeparam>
	template<typename T>
	class IdPool
	{
	private:
		IdPoolStackAdapter<T> freeIds{ };
		T start;
		T current;
		T step;

	public:
		/// <summary>
		/// Creates a new ID pool.
		/// </summary>
		/// <param name="_start">The minimum value at which the Pool starts handing out IDs (defaults to 0)</param>
		/// <param name="_step">The numerical difference between each ID (defaults to 1)</param>
		/// <param name="reservedFrees">reservedFrees Amount of IDs that can be returned to the pool before a reallocation happens (defaults to 32)</param>
		IdPool(T _start = static_cast<T>(0), T _step = static_cast<T>(1), size_t reservedFrees = 32)
		{
			start = _start;
			current = start;
			step = _step;
			freeIds.GetContainer().reserve(reservedFrees);
		}

		/// <summary>
		/// Get an unused ID from the pool.
		/// </summary>
		/// <returns>An unused ID</returns>
		T Next()
		{
			T id{ };
			if (freeIds.empty())
			{
				id = current;
				current = current + static_cast<T>(1);
				return id;
			}
			id = freeIds.top();
			freeIds.pop();
			return id;
		}

		/// <summary>
		/// Return an ID back to the pool (unchecked). Only pass IDs that have been returned by this->Next()!
		/// </summary>
		/// <param name="id">The ID to put back into the pool</param>
		void Free(T id)
		{
			if (id == current - static_cast<T>(1)) current = current - static_cast<T>(1);
			else freeIds.push(id);
		}
	};
}
