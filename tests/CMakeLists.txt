find_package(Qt6 REQUIRED COMPONENTS Test)

function(add_kanooppi_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE
        Qt6::Test
        Qt6::Core
        KanoopPiQt
    )
    target_include_directories(${name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
    )
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_kanooppi_test(tst_smoke)
add_kanooppi_test(tst_mockbackend)
add_kanooppi_test(tst_outputbank)
add_kanooppi_test(tst_outputpin)
