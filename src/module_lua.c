// module_lua.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "module_lua.h"

// Initialize Lua and load script if it exists
bool module_init_lua(const char* script_file, int argc, char* argv[], LuaData* lua_data) {
    lua_data->L = luaL_newstate(); // Create a new Lua state
    if (!lua_data->L) {
        fprintf(stderr, "Failed to create Lua state\n");
        return false;
    }

    // Open standard Lua libraries
    luaL_openlibs(lua_data->L);

    // Push command-line arguments to Lua global table "arg"
    lua_newtable(lua_data->L);
    for (int i = 0; i < argc; i++) {
        lua_pushstring(lua_data->L, argv[i]);
        lua_rawseti(lua_data->L, -2, i);
    }
    lua_setglobal(lua_data->L, "arg");

    // Check if script file exists
    FILE* file = fopen(script_file, "r");
    if (!file) {
        printf("Lua script '%s' not found, ignoring\n", script_file);
        lua_data->update_ref = LUA_NOREF; // No update function
        return true; // Continue without script
    }
    fclose(file);

    // Load and execute the Lua script
    if (luaL_dofile(lua_data->L, script_file) != LUA_OK) {
        fprintf(stderr, "Error loading Lua script '%s': %s\n", script_file, lua_tostring(lua_data->L, -1));
        lua_pop(lua_data->L, 1);
        lua_close(lua_data->L);
        lua_data->L = NULL;
        return false;
    }

    // Get the update function and store it in the registry
    lua_getglobal(lua_data->L, "update");
    if (lua_isfunction(lua_data->L, -1)) {
        lua_data->update_ref = luaL_ref(lua_data->L, LUA_REGISTRYINDEX); // Store reference
    } else {
        lua_pop(lua_data->L, 1); // Pop non-function value
        lua_data->update_ref = LUA_NOREF; // No update function
        printf("No 'update' function found in '%s'\n", script_file);
    }

    return true;
}

// Call the Lua update function with delta time
void module_update_lua(LuaData* lua_data, float dt) {
    if (lua_data->L && lua_data->update_ref != LUA_NOREF) {
        lua_rawgeti(lua_data->L, LUA_REGISTRYINDEX, lua_data->update_ref); // Get update function
        lua_pushnumber(lua_data->L, dt); // Push delta time
        if (lua_pcall(lua_data->L, 1, 0, 0) != LUA_OK) {
            fprintf(stderr, "Error calling Lua update: %s\n", lua_tostring(lua_data->L, -1));
            lua_pop(lua_data->L, 1);
        }
    }
}

// Cleanup Lua resources
void module_cleanup_lua(LuaData* lua_data) {
    if (lua_data->L) {
        if (lua_data->update_ref != LUA_NOREF) {
            luaL_unref(lua_data->L, LUA_REGISTRYINDEX, lua_data->update_ref);
            lua_data->update_ref = LUA_NOREF;
        }
        lua_close(lua_data->L);
        lua_data->L = NULL;
    }
}