# Autogenerated file, run tools/build/setup_cmake.py to regenerate

include_directories(%(includepath)s)
link_directories(%(libpath)s)

set(pytests %(pyexamples)s)

foreach (test ${pytests})
 GET_FILENAME_COMPONENT(name ${test} NAME_WE)
 add_test(${name} ${PROJECT_BINARY_DIR}/imppy.sh "python" ${test})
 set_tests_properties(${name} PROPERTIES LABELS %(name)s-example)
  #add_dependencies(${name} RMFPython)
endforeach(test)

set(cpp_tests %(cppexamples)s)

foreach (test ${cpp_tests})
   GET_FILENAME_COMPONENT(name ${test} NAME_WE)
   add_executable("${name}" ${test})
   target_link_libraries(${name}     imp_%(name)s
    %(modules)s
    %(dependencies)s)
   set_target_properties(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/test/%(name)s/")
   add_test(${name} ${PROJECT_BINARY_DIR}/imppy.sh "${PROJECT_BINARY_DIR}/test/%(name)s/${name}")
   set_tests_properties(${name} PROPERTIES LABELS %(name)s-example)
endforeach(test)