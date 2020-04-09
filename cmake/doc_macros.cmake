# clear variable to avoid accumulation
set(SMARTPEAK_DOCUMENTATION_DIRECTORIES ""
  CACHE
  INTERNAL
  "List of paths to be searched when the API documentation is generated")


#------------------------------------------------------------------------------
# Registers the given path in the documentation system
#
# @param path_to_document Path containing header files that should be documented
macro(smartpeak_doc_path path_to_document)
  set(SMARTPEAK_DOCUMENTATION_DIRECTORIES
    ${path_to_document}
    ${SMARTPEAK_DOCUMENTATION_DIRECTORIES}
    CACHE
    INTERNAL
    "List of paths to be searched when the API documentation is generated")
endmacro()
