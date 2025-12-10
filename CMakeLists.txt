cmake_minimum_required(VERSION 3.5)
project(ProgramacaoParalelaSO)

# Configurar o SDL3 antes de incluí-lo
set(SDL_SHARED OFF CACHE BOOL "Desabilitar a build da biblioteca dinâmica do SDL3.")
set(SDL_STATIC ON CACHE BOOL "Habilitar a build da biblioteca estática do SDL3.")

# Adicionar submodulo do SDL3 como dependência externa
add_subdirectory(external/sdl EXCLUDE_FROM_ALL)

# Adicionar o diretório 'include/' como um diretório de cabeçalhos
include_directories(include)

# Criar executável das questões 1 e 2
add_executable(scalar_product
    src/main.c
    src/scalar_product.c
    src/utils.c
)

# Criar executável da questão 3 linkado com SDL3
add_executable(simulation
    src/simulation.c
)
target_link_libraries(simulation PRIVATE SDL3::SDL3-static)