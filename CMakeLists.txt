cmake_minimum_required(VERSION 3.5)
project(TrabalhoSO2 C)

# Configurar o SDL3 antes de incluí-lo
set(SDL_SHARED OFF CACHE BOOL "Desabilitar a build da biblioteca dinâmica do SDL3.")
set(SDL_STATIC ON CACHE BOOL "Habilitar a build da biblioteca estática do SDL3.")

# Adicionar submodulo do SDL3 como dependência externa
add_subdirectory(external/sdl EXCLUDE_FROM_ALL)

# Adicionar `include/` como diretório de cabeçalhos
include_directories(include)
link_libraries(pthread)

# Adicionar executável da parte 1
add_executable(scalar_product
    src/main.c
    src/scalar_product.c
    src/utils.c
)

# Adicionar executável da parte 2
add_executable(matrix_product
    src/main.c
    src/matrix_product.c
    src/utils.c
)

# Adicionar executável da parte 3
add_executable(simulation
    src/simulation.c
)

# Linkar parte 3 com o SDL
target_link_libraries(simulation PRIVATE SDL3::SDL3-static)
