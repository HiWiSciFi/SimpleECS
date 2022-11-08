#include <iostream>
#include <chrono>
#include "ECS.hpp"

struct Transform
{
    float x, y, z;
};

class Gravity : public ECS::System
{
public:
    virtual void Init() override
    {
        RequireComponent<Transform>();
    }

    virtual void Update(float dt) override
    {
        for (auto& e : entities)
        {
            Transform& transform = e.GetComponent<Transform>();
            transform.y -= 1.0f * dt;
            fprintf(stdout, "Position: (%f, %f, %f) dt: %f\n", transform.x, transform.y, transform.z, dt);
        }
    }
};

int main()
{
    std::cout << "Hello World!\n";

    ECS::RegisterComponent<Transform>();
    auto gravity = ECS::RegisterSystem<Gravity>();

    ECS::Entity e = ECS::CreateEntity();
    e.AddComponent<Transform>();

    float dt = 0.0f;
    while (true)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        gravity->Update(dt);
        dt = std::chrono::duration<float, std::chrono::seconds::period>(
            std::chrono::high_resolution_clock::now() - startTime).count();
    }
}
