TEMPLATE = app
TARGET = scl-feature-benchmark-default-tool

DEPENDS *= \
    scl-feature \

!load( include_files ) : error( Can not find feature \"include_files\" )
!load( module_depends ) : error( Can not find feature \"module_depends\" )

includeFiles( $${PWD}/../../../../benchmark/DefaultTool )
