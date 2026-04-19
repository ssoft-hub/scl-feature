#!/bin/sh
# Build versioned Doxygen docs for CI deployment.
# Must be run from the module root directory.
#
# Usage: sh script/ci/deploy_versioned_docs.sh [output_dir]
#   output_dir — defaults to public/ (GitLab/GitHub Pages artifact)

exec sh script/doxygen/build.sh "${1:-public}"
