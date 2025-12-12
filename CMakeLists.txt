cmake_minimum_required(VERSION 3.12)
project(TrabalhoSO2 C)

# Buscar biblioteca de threads (`pthread`)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Adicionar `include/` como diretório de cabeçalhos
include_directories(include)

# Adicionar executável da parte 1
add_executable(scalar_product
    src/main-sp.c
    src/scalar_product.c
    src/utils.c
)

# Adicionar executável da parte 2
add_executable(matrix_product
    src/main-mp.c
    src/matrix_product.c
    src/utils.c
)

# Adicionar executável da parte 3
# add_executable(simulation
#     src/simulation.c
# )

link_libraries(scalar_product PRIVATE Threads::Threads)
link_libraries(matrix_product PRIVATE Threads::Threads)
#link_libraries(simulation PRIVATE Threads::Threads)