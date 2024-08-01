TEMPLATE = app
TARGET = scl-feature-example-inheritance
CONFIG *= console

DEPENDS *= \
    scl-feature \

!load( include_files ) : error( Can not find feature \"include_files\" )
!load( module_depends ) : error( Can not find feature \"module_depends\" )

includeFiles( $${PWD}/../../../../example/Inheritance )
