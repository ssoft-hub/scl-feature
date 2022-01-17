TEMPLATE = lib
CONFIG *= static

!load( include_files ) : error( Can not find feature \"include_files\" )
includeFiles( $${PWD}/../../include )
includeFiles( $${PWD}/../../src )
includeFiles( $${PWD}/../../doc/ru )

OTHER_FILES *= \
    $$files( $${PWD}/*.prf ) \
