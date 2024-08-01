TEMPLATE = lib
CONFIG *= static

!load( include_files ) : error( Can not find feature \"include_files\" )
!load( module_depends ) : error( Can not find feature \"module_depends\" )

includeFiles( $${PWD}/../../include )
includeFiles( $${PWD}/../../src )
includeFiles( $${PWD}/../../doc/ru )

QMAKE_TARGET_COMPANY = "SSoft"
QMAKE_TARGET_COPYRIGHT = "Andrei Stepanov, 2022"
QMAKE_TARGET_LICENSE = "Free and Unencumbered Software License"

OTHER_FILES *= \
    $$files( $${PWD}/*.prf ) \
