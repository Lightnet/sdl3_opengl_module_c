// module_flecs.h
// #pragma once

// module_flecs.h
#ifndef MODULE_FLECS_H
#define MODULE_FLECS_H

#include <flecs.h> // Flecs 4.1 header (ensure it's in your include path)

// Simple test components
typedef struct {
    float x, y;
} Position;

typedef struct {
    float dx, dy;
} Velocity;

// Simple system to test phases (runs in EcsOnUpdate phase)
void MoveSystem(ecs_iter_t *it);

typedef struct {
    ecs_world_t *world;
    ecs_entity_t test_entity; // Example entity with Position and Velocity
} FlecsData;

bool module_init_flecs(FlecsData *flecs_data);
void module_update_flecs(FlecsData *flecs_data, float dt);
void module_cleanup_flecs(FlecsData *flecs_data);

#endif // MODULE_FLECS_H