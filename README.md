# sdl3_opengl_module_c

# License: MIT

# Libraries:
- sdl 3.2
- cimgui 1.92.1
- stb
- cglm 0.9.6
- enet ( n/a )
- flecs ( n/a )
- lua ( n/a )

# Status:
- work in progress
- need to build this module for cimgui, lua and others.
- network.

# Information:
  Very simple SDL 3.2.22 cimgui. OpenGL Glad 330.

# cmake:
 Note that cimgui has cmake build which should not be used. Still need to find correct way to config.

CMakeLists.txt cimgui
```
#general settings
file(GLOB IMGUI_SOURCES
    cimgui.cpp
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_widgets.cpp
	${TABLES_SOURCE}
)
```

# cimgui api
-build\_deps\cimgui-src\cimgui_impl.h  
    - api cimgui
```
#ifdef CIMGUI_USE_OPENGL3
CIMGUI_API bool ImGui_ImplOpenGL3_Init(const char* glsl_version);
CIMGUI_API void ImGui_ImplOpenGL3_Shutdown(void);
CIMGUI_API void ImGui_ImplOpenGL3_NewFrame(void);
CIMGUI_API void ImGui_ImplOpenGL3_RenderDrawData(ImDrawData* draw_data);
CIMGUI_API bool ImGui_ImplOpenGL3_CreateDeviceObjects(void);
CIMGUI_API void ImGui_ImplOpenGL3_DestroyDeviceObjects(void);
CIMGUI_API void ImGui_ImplOpenGL3_UpdateTexture(ImTextureData* tex);
```

# Credits:
  - cimgui
  - imgui
  - Grok AI on x
  - https://kenney.nl/assets/kenney-fonts