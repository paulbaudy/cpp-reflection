# CMakeList.txt : projet CMake pour Reflection, incluez la source et définissez
# la logique spécifique au projet ici.
#
cmake_minimum_required (VERSION 3.8)

# Activez Rechargement à chaud pour les compilateurs MSVC si cela est pris en charge.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Reflection")

# Ajoutez une source à l'exécutable de ce projet.
add_executable (Reflection "Reflection.cpp" "Reflection.h" "Field.h" "StaticString.h" "LayoutIterator.h" "TupleVisitor.h" "Tuple.h" "Layout.h" "LayoutView.h" "Name.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Reflection PROPERTY CXX_STANDARD 20)
endif()

# TODO: Ajoutez des tests et installez des cibles si nécessaire.
