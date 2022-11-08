#pragma once

#include <bitset>
#include <deque>
#include <inttypes.h>
#include <memory>
#include <queue>
#include <set>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

namespace ECS
{
	constexpr uint8_t MAX_COMPONENTS = 64;
	using EntityType = size_t;
	class Entity;
	static size_t componentCount = 0;

	class IComponentArray
	{
	public:
		virtual ~IComponentArray() = default;
	};

	template<typename T>
	class ComponentArray : public IComponentArray
	{
	private:
		std::vector<T> componentArray{};
		std::unordered_map<EntityType, size_t> entityToIndexMap{};
		std::unordered_map<size_t, EntityType> indexToEntityMap{};

	public:
		void Insert(EntityType e, T component)
		{
			size_t index = componentArray.size();
			componentArray.push_back(component);
			entityToIndexMap[e] = index;
			indexToEntityMap[index] = e;
		}

		void Erase(EntityType e)
		{
			componentArray[entityToIndexMap[e]] = componentArray.back();
			componentArray.pop_back();
		}

		T& Get(EntityType e)
		{
			return componentArray[entityToIndexMap[e]];
		}
	};

	static std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> componentArrays{};

	class Entity
	{
	private:
		EntityType id;

	public:
		Entity(EntityType _id) : id(_id) { }

		EntityType GetID() { return id; }

		template<typename T>
		void AddComponent(T component) const
		{
			std::type_index componentId = typeid(T);
			std::static_pointer_cast<ComponentArray<T>>(componentArrays[componentId])->Insert(id, component);
		}

		template<typename T>
		void AddComponent() const { AddComponent(T{ }); }

		template<typename T>
		void RemoveComponent() const
		{
			std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeid(T)])->Erase(id);
		}

		/*template<typename T>
		std::shared_ptr<ComponentArray<T>> GetComponentArray()
		{
			return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeid(T)]);
		}*/

		template<typename T>
		T& GetComponent() const
		{
			return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeid(T)])->Get(id);
		}

		operator EntityType() const { return id; }
	};

	class System
	{
	protected:
		template<typename T>
		void RequireComponent()
		{
			requirements.push_back(typeid(T));
		}

	public:
		std::set<Entity> entities{};
		std::vector<std::type_index> requirements{};
		virtual void Init() { }
		virtual void Update(float dt) { }
	};

	static std::set<std::shared_ptr<System>> systems{};
	static std::vector<std::bitset<MAX_COMPONENTS>> entities{};
	static std::queue<EntityType> freeEntities{};

	Entity CreateEntity()
	{
		EntityType id;
		if (freeEntities.empty())
		{
			id = entities.size();
		}
		else
		{
			id = freeEntities.front();
			freeEntities.pop();
		}
		while (id >= entities.size()) entities.push_back(std::bitset<MAX_COMPONENTS>());
		entities[id] = id;
		Entity e = Entity(id);
		for (auto& s : systems) { s->entities.insert(e); }
		return e;
	}

	void DeleteEntity(Entity e)
	{
		freeEntities.push(e);
		entities[e].reset();
	}

	template<typename T>
	std::shared_ptr<System> RegisterSystem()
	{
		std::shared_ptr<System> system = std::make_shared<T>();
		systems.insert(system);
		system->Init();
		return system;
	}

	template<typename T>
	void RegisterComponent()
	{
		componentArrays.insert({ typeid(T), std::make_shared<ComponentArray<T>>() });
	}
}
