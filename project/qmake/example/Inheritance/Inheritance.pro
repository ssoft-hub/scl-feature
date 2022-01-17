TEMPLATE = app
TARGET = scl-feature-example-inheritance
CONFIG *= console

DEPENDS *= \
    scl-feature \

!load( include_files ) : error( Can not find feature \"include_files\" )
includeFiles( $${PWD}/../../../../example/Inheritance )
