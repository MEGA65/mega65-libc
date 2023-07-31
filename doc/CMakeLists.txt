find_package(Doxygen)
set(Python3_FIND_VIRTUALENV "ONLY")
find_package(Python3 COMPONENTS Interpreter)

if(DOXYGEN_FOUND)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} -q ${CMAKE_SOURCE_DIR}/doc/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )

    if(Python3_Interpreter_FOUND)
        add_custom_target(guide
            COMMAND ${Python3_EXECUTABLE} m65guide.py xml/conio_8h.xml > api-conio.tex
            COMMAND ${Python3_EXECUTABLE} m65guide.py xml/dirent_8h.xml > api-dirent.tex
            COMMAND ${Python3_EXECUTABLE} m65guide.py xml/fcio_8h.xml > api-fcio.tex
            COMMAND ${Python3_EXECUTABLE} m65guide.py xml/fileio_8h.xml > api-fileio.tex
            COMMAND ${Python3_EXECUTABLE} m65guide.py xml/hal_8h.xml > api-hal.tex
            COMMAND ${Python3_EXECUTABLE} m65guide.py xml/memory_8h.xml > api-memory.tex
            COMMAND ${Python3_EXECUTABLE} m65guide.py xml/random_8h.xml > api-random.tex
            COMMAND ${Python3_EXECUTABLE} m65guide.py xml/tests_8h.xml > api-tests.tex
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/doc
            COMMENT "Generating LaTeX API docs for the MEGA65 User Guide" VERBATIM
        )
        add_dependencies(guide doc)
    endif(Python3_Interpreter_FOUND)
endif(DOXYGEN_FOUND)

