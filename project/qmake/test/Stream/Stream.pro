TEMPLATE = app
TARGET = scl-feature-test-stream
CONFIG *= console

DEPENDS *= \
    scl-feature \

!load( include_files ) : error( Can not find feature \"include_files\" )
includeFiles( $${PWD}/../../../../test/Stream )
