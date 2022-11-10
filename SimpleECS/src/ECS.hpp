#pragma once

#include <memory>
#include <set>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

#include "ComponentContainer.hpp"
#include "IdPool.hpp"

namespace ECS
{
	using EntityType = size_t;
	class System;

	std::unordered_map<std::type_index, std::shared_ptr<ComponentContainer>> componentStores{ };
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems{ };
	IdPool<EntityType> idPool{ };

	template<typename T>
	std::shared_ptr<ComponentStore<T>> GetComponentStore()
	{
		return std::static_pointer_cast<ComponentStore<T>>(componentStores[typeid(T)]);
	}

	/// <summary>
	/// A wrapper for Entity IDs (type: EntityType) to make entity-related functions more accessible
	/// </summary>
	class Entity
	{
	private:

		/// <summary>
		/// The ID for this entity
		/// </summary>
		EntityType id;

	public:

		/// <summary>
		/// Create a entity from an ID
		/// </summary>
		/// <param name="_id">The ID of the entity</param>
		Entity(EntityType _id) : id(_id) { }

		/// <summary>
		/// Get the ID of the entity
		/// </summary>
		/// <returns>The ID of the entity</returns>
		EntityType GetId() const { return id; }

		/// <summary>
		/// Enables casting of the entity class to a number
		/// </summary>
		operator EntityType() const { return id; }

		/// <summary>
		/// Adds a component to the entity
		/// </summary>
		/// <typeparam name="T">The component type to add</typeparam>
		/// <param name="component">The component data to add</param>
		template<typename T>
		void AddComponent(T component) const
		{
			GetComponentStore<T>()->Insert(id, component);
		}

		/// <summary>
		/// Add a component to the entity
		/// </summary>
		/// <typeparam name="T">The component type to add (zero-initialized)</typeparam>
		template<typename T>
		void AddComponent() const
		{
			GetComponentStore<T>()->Insert(id, T{ });
		}

		/// <summary>
		/// Removes a component from the entity
		/// </summary>
		/// <typeparam name="T">The component to remove</typeparam>
		template<typename T>
		void RemoveComponent() const
		{
			GetComponentStore<T>()->Erase(id);
		}

		/// <summary>
		/// Checks if the entity has a component
		/// </summary>
		/// <typeparam name="T">The component type to check for</typeparam>
		/// <returns>true if the entity has the component, false otherwise</returns>
		template<typename T>
		bool HasComponent() const
		{
			GetComponentStore<T>()->HasStored(id);
		}

		/// <summary>
		/// Gets a component from the entity
		/// </summary>
		/// <typeparam name="T">The component to get</typeparam>
		/// <returns>The specified component</returns>
		template<typename T>
		T& GetComponent() const
		{
			return GetComponentStore<T>()->Get(id);
		}
	};

	/// <summary>
	/// Superclass for all Systems to be used by the ECS
	/// </summary>
	class System
	{
	protected:

		/// <summary>
		/// Set of components that need to be present for an entity to be handled by this system
		/// </summary>
		std::set<std::type_index> requirements{ };

		/// <summary>
		/// Sets a component requirement for this system
		/// </summary>
		/// <typeparam name="T">The component type to require</typeparam>
		template<typename T>
		void RequireComponent()
		{
			requirements.insert(typeid(T));
		}

	public:

		/// <summary>
		/// All entities that are relevant to the system
		/// </summary>
		std::set<Entity> entities;

		/// <summary>
		/// Get the requirements of the system
		/// </summary>
		/// <returns>A set of std::type_index containing all required types</returns>
		const std::set<std::type_index>& GetRequirements() const
		{
			return requirements;
		}

		/// <summary>
		/// Called upon System Registration -- Register component requirements here
		/// </summary>
		virtual void Init() { }

		/// <summary>
		/// Called once each update frame
		/// </summary>
		/// <param name="dt">The time in seconds that has passed since the last call</param>
		virtual void Update(float dt) { }
	};

	template<typename T>
	void RegisterComponent()
	{
		componentStores.insert({ typeid(T), std::make_shared<ComponentStore<T>>() });
	}

	template<typename T>
	std::shared_ptr<System> RegisterSystem()
	{
		std::shared_ptr<System> system = std::make_shared<T>();
		systems.insert({ typeid(T), system });
		system->Init();
		return system;
	}

	Entity CreateEntity()
	{
		Entity e(idPool.Next());
		for (auto const& system : systems)
		{
			system.second->entities.insert(e);
		}
		return e;
	}

	void DestroyEntity(Entity e)
	{
		idPool.Free(e.GetId());
	}
}
