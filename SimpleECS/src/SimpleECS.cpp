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
            //Transform& transform = e.GetComponent<Transform>();
            //transform.y -= 1.0f * dt;
        }
    }
};

int main()
{
    std::cout << "Hello World!\n";

    ECS::RegisterComponent<Transform>();
    auto gravity = ECS::RegisterSystem<Gravity>();

    auto startTime = std::chrono::high_resolution_clock::now();

    for (int k = 0; k < 1024; k++)
    {
        for (int i = 0; i < 1024; i++)
        {
             ECS::Entity e = ECS::CreateEntity();
            e.AddComponent<Transform>();
        }
        for (size_t i = 0; i < 1024; i++) ECS::DestroyEntity(i);
        while(0);
    }

    float setupTime = std::chrono::duration<float, std::chrono::milliseconds::period>(
            std::chrono::high_resolution_clock::now() - startTime).count();
    fprintf(stdout, "Setup time: %fms\n", setupTime);

    float dt = 0.0f;
    while (0)
    {
        startTime = std::chrono::high_resolution_clock::now();
        gravity->Update(dt);
        dt = std::chrono::duration<float, std::chrono::seconds::period>(
            std::chrono::high_resolution_clock::now() - startTime).count();
        printf("Loop\n");
    }
    printf("End\n");
}
