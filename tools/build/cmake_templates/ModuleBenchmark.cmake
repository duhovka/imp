# Autogenerated file, run tools/build/setup_cmake.py to regenerate

include_directories(%(includepath)s)
link_directories(%(libpath)s)

set(cppbenchmarks %(cppbenchmarks)s)

foreach (bin ${cppbenchmarks})
   GET_FILENAME_COMPONENT(name ${bin} NAME_WE)
   add_executable("${name}" ${bin})
   target_link_libraries(${name}     imp_%(name)s
    %(modules)s
    %(dependencies)s)
   set_target_properties(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/benchmark/%(name)s")
endforeach(bin)