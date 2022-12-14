#pragma once

#include <unordered_map>
#include <vector>

#define COMPONENT_USE_SINGLE_MAP

namespace ECS
{
	using EntityType = size_t;

	/// <summary>
	/// A container for the ComponentStore&lt;T&gt;
	/// </summary>
	class ComponentContainer
	{
	public:
		virtual ~ComponentContainer() = default;

		virtual void Insert(EntityType e) = 0;
		virtual void Erase(EntityType e) = 0;
		virtual bool HasStored(EntityType e) = 0;
		virtual size_t GetEntities(EntityType* buffer) = 0;
	};

	/// <summary>
	/// A store for a component array
	/// </summary>
	/// <typeparam name="T">The component this is a store for</typeparam>
	template<typename T>
	class ComponentStore : public ComponentContainer
	{
	private:
		std::vector<T> componentArray{ };
		std::unordered_map<EntityType, size_t> entityToIndexMap{ };
		#ifndef COMPONENT_USE_SINGLE_MAP
		std::unordered_map<size_t, EntityType> indexToEntityMap{ };
		#endif

	public:
		ComponentStore() = default;

		/// <summary>
		/// Store a new component
		/// </summary>
		/// <param name="e">The entity to add the component for</param>
		/// <param name="component">The component to store</param>
		void Insert(EntityType e, T component)
		{
			size_t index = componentArray.size();
			componentArray.push_back(component);
			entityToIndexMap.insert({ e, index });
			#ifndef COMPONENT_USE_SINGLE_MAP
			indexToEntityMap.insert({ index, e });
			#endif
		}

		/// <summary>
		/// Store a new component
		/// </summary>
		/// <param name="e">The entity to add the component for</param>
		virtual void Insert(EntityType e) override
		{
			this->Insert(e, T{ });
		}

		/// <summary>
		/// Removes the component stored for a specific entity
		/// </summary>
		/// <param name="e">The entity the component has been stored for</param>
		virtual void Erase(EntityType e) override
		{
			if (entityToIndexMap.find(e) == entityToIndexMap.end()) return;
			size_t index = entityToIndexMap.at(e);
			componentArray[index] = componentArray.back();
			componentArray.pop_back();

			#ifdef COMPONENT_USE_SINGLE_MAP
			for (auto& element : entityToIndexMap)
			{
				if (element.second == componentArray.size()) element.second = index;
			}
			#endif
			entityToIndexMap.erase(e);
			#ifndef COMPONENT_USE_SINGLE_MAP
			indexToEntityMap.erase(index);
			#endif
		}

		/// <summary>
		/// Get the component for a specific entity
		/// </summary>
		/// <param name="e">The entity whose component to get</param>
		/// <returns>The component that has been stored for the entity</returns>
		T& Get(EntityType e)
		{
			return componentArray.at(entityToIndexMap.at(e));
		}

		/// <summary>
		/// Check if there is a component stored for a specific entity
		/// </summary>
		/// <param name="e">The entity whose component to find</param>
		/// <returns>true if there is a component stored for the entity, false otherwise</returns>
		virtual bool HasStored(EntityType e) override
		{
			return entityToIndexMap.find(e) != entityToIndexMap.end();
		}

		/// <summary>
		/// Get all entities which have a component stored. Call with NULL/nullptr as buffer to get amount of entities
		/// </summary>
		/// <param name="buffer">The buffer to write to or NULL/nullptr</param>
		/// <returns>Amount of entities</returns>
		virtual size_t GetEntities(EntityType* buffer) override
		{
			if (buffer == nullptr || buffer == NULL) return entityToIndexMap.size();
			size_t i = 0;
			for (auto const& pair : entityToIndexMap)
			{
				buffer[i] = pair.first;
				i++;
			}
			return i == 0 ? i : i - 1;
		}
	};
}
