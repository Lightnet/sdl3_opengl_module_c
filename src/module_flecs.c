// module_flecs.c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "module_flecs.h"

void MoveSystem(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0); // Column 0: Position
    Velocity *v = ecs_field(it, Velocity, 1); // Column 1: Velocity
    for (int i = 0; i < it->count; i++) {
        p[i].x += v[i].dx * it->delta_time;
        p[i].y += v[i].dy * it->delta_time;
        printf("Entity %lu moved to (%.2f, %.2f)\n", (unsigned long)it->entities[i], p[i].x, p[i].y); // Simple test output
    }
}

// Initialize Flecs world, components, system, and test entity
bool module_init_flecs(FlecsData *flecs_data) {
    flecs_data->world = ecs_init();
    if (!flecs_data->world) {
        fprintf(stderr, "Failed to initialize Flecs world\n");
        return false;
    }

    // Register components (startup phase)
    ECS_COMPONENT(flecs_data->world, Position);
    ECS_COMPONENT(flecs_data->world, Velocity);

    // Create system in EcsOnUpdate phase (simple phase test)
    ECS_SYSTEM(flecs_data->world, MoveSystem, EcsOnUpdate, Position, Velocity);

    // Create a test entity with components (startup test)
    flecs_data->test_entity = ecs_new(flecs_data->world);
    ecs_set(flecs_data->world, flecs_data->test_entity, Position, {10.0f, 20.0f});
    ecs_set(flecs_data->world, flecs_data->test_entity, Velocity, {1.0f, 2.0f});

    printf("Flecs initialized: Test entity created at (10.0, 20.0) with velocity (1.0, 2.0)\n");
    return true;
}

// Update Flecs world (progresses through phases, calls systems with dt)
void module_update_flecs(FlecsData *flecs_data, float dt) {
    if (flecs_data->world) {
        ecs_progress(flecs_data->world, dt);
    }
}

// Cleanup Flecs resources
void module_cleanup_flecs(FlecsData *flecs_data) {
    if (flecs_data->world) {
        ecs_fini(flecs_data->world);
        flecs_data->world = NULL;
        flecs_data->test_entity = 0;
    }
    printf("Flecs cleaned up\n");
}