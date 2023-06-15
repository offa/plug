
function(deprecate_options)
  foreach (arg IN LISTS ARGN)
      if (DEFINED ${arg})
          message(WARNING "Option ${arg} is deprecated, please use PLUG_${arg} instead, value will be ignored!")
      endif()
  endforeach()
endfunction()

deprecate_options(UNITTEST
    COVERAGE
    LTO
    SANITIZER_ASAN
    SANITIZER_UBSAN
    )
