TEMPLATE = app
TARGET = scl-feature-example-reflection

DEPENDS *= \
    scl-feature \

!load( module_depends ) : error( Can not find feature \"module_depends\" )
!load( include_files ) : error( Can not find feature \"include_files\" )
includeFiles( $${PWD}/../../../../example/Reflection )
