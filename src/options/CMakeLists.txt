
set(SOURCES)

add_library(glb-lib-options OBJECT
    options.c
    help-opt.c
    cstring.c
)

add_library(glb-lib-options_static STATIC
       $<TARGET_OBJECTS:glb-lib-options>)

