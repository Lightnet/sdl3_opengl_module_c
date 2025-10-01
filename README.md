# sdl3_opengl_module_c

# License: MIT

# Libraries:
- sdl 3.2
- cimgui 1.92.1
- glad 2.0.8 ( opengl 330) 
- stb ( stb_image.h / stb_truetype.h ) https://github.com/nothings/stb
- cglm 0.9.6
- enet ( n/a )
- flecs 4.1 ( testing )
- lua 5.4 ( testing )

# Status:
- work in progress
- need to build this module for cimgui, lua and others.
- work on progress on docs.

# Information:
  Very simple SDL 3.2.22 cimgui. OpenGL Glad 330. It sample tests and prototype to make it simple as possible for sandbox. Using the AI agent to help build building blocks and docs. https://x.com/i/grok For easy to protoype builds and understand of the logics of design and write docs.

# Samples:
  Working on the sample to build sample of vary type of builds and note there will be over lap due to do stb_truetype.h and imgui modifed imstb_truetype.h.

## list: 
 - Work toward keep it simple as possible for testing the shaders.
 - font loading using the stb_truetype.
 - cimgui loading for create widget for debugging.
 - quad 2d 
 - 3d cube with texture pixel nearest and no light (wip)
 - lua (wip)
 - flecs (wip)
 - transform 3d hierarchy (wip)

# Reason for c programing language:
  It very simple practice and understand programing the basic build. As well refine the code with limited design. Just like the fantasy console specs restricted.

# cmake:
  Using the windows msys64 tool compile.

# Credits:
  - cimgui https://github.com/cimgui/cimgui
  - imgui https://github.com/ocornut/imgui
  - Grok AI on x
  - https://kenney.nl/assets/kenney-fonts