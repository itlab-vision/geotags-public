add_library(tg STATIC
    tg.c tg.h
)
target_compile_options(tg PRIVATE -std=gnu11)
set_target_properties(tg PROPERTIES
    LINKER_LANGUAGE C
)
