cmake_minimum_required(VERSION 3.12)
project(TrabalhoSO2 C)

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
# add_executable(simulation
#     src/simulation.c
# )