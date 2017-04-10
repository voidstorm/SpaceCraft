
function(COFFEEPresentationAddCopyICCLibsTarget)
    if(UNIX)
        if (CMAKE_COMPILER_IS_GNUCXX)
        else() # we assume icc
            get_filename_component(ICC_LIBRARY_PATH ${CMAKE_C_COMPILER}/../../../compiler/lib/ia32 ABSOLUTE)

            list(APPEND ICC_LIBRARIES libchkp.so)
            list(APPEND ICC_LIBRARIES libcilkrts.so)
            list(APPEND ICC_LIBRARIES libcilkrts.so.5)
            list(APPEND ICC_LIBRARIES libimf.so)
            list(APPEND ICC_LIBRARIES libintlc.so)
            list(APPEND ICC_LIBRARIES libintlc.so.5)
            list(APPEND ICC_LIBRARIES libiomp5.so)
            list(APPEND ICC_LIBRARIES libiompstubs5.so)
            list(APPEND ICC_LIBRARIES libirc.so)
            list(APPEND ICC_LIBRARIES libirng.so)
            list(APPEND ICC_LIBRARIES libomp_db.so)
            list(APPEND ICC_LIBRARIES libpdbx.so)
            list(APPEND ICC_LIBRARIES libpdbx.so.5)
            list(APPEND ICC_LIBRARIES libsvml.so)

            COFFEEAddCopyFilesTarget(xx_copy_icc x50_copy_targets "${ICC_LIBRARIES}"
                                        "${ICC_LIBRARY_PATH}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")
        endif()
    endif()
endfunction(COFFEEPresentationAddCopyICCLibsTarget)

