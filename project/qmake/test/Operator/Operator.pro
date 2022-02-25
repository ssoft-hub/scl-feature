TEMPLATE = app
TARGET = scl-feature-test-operator
CONFIG *= console

DEPENDS *= \
    scl-feature \

!load( include_files ) : error( Can not find feature \"include_files\" )
includeFiles( $${PWD}/../../../../test/Operator )
