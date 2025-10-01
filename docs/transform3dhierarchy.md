transform 3d hierarchy


# order for child self rotate:
```
glm_translate(local, transform->position); // Translate first
glm_quat_mat4(transform->rotation, rot);
glm_mat4_mul(local, rot, local); // Rotate second
glm_scale(local, transform->scale); // Scale last
```
