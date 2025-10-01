// module_lua.h
#ifndef MODULE_LUA_H
#define MODULE_LUA_H

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

typedef struct {
    lua_State* L; // Lua state
    int update_ref; // Reference to the Lua update function
} LuaData;

bool module_init_lua(const char* script_file, int argc, char* argv[], LuaData* lua_data);
void module_update_lua(LuaData* lua_data, float dt);
void module_cleanup_lua(LuaData* lua_data);

#endif // MODULE_LUA_H