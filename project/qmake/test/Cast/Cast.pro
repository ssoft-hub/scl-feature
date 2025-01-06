cdTEMPLATE = app
TARGET = scl-feature-test-cast
CONFIG *= console

DEPENDS *= \
    scl-feature \
    doctest \

!load( module_depends ) : error( Can not find feature \"module_depends\" )
!load( include_files ) : error( Can not find feature \"include_files\" )
includeFiles( $${PWD}/../../../../test/Cast )
