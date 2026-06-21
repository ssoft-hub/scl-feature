#!/usr/bin/env bash
#
# cppcheck lint — single source of truth for CI and local (IDE) runs.
# Mirrors the cppcheck job in .gitlab-ci.yml / .github/workflows/lint.yml.
# Uses the project .cppcheck suppressions list.
#
# Needs the scl-utility headers on the include path. Resolution order:
#   1. $SCL_UTILITY_SRC if set
#   2. ../utility/src    (live submodule in the monorepo / IDE)
#   3. /tmp/scl-utility/src (clone created by CI)
# The script never clones and never writes to /tmp.
#
# Env overrides:
#   SCL_SRC_DIR      source directory to scan (default: src)
#   SCL_UTILITY_SRC  scl-utility src directory (default: auto-detected)
#   CPPCHECK         cppcheck executable (default: cppcheck on PATH)
#
set -euo pipefail
cd "$(dirname "$0")/../.."

SCL_SRC_DIR="${SCL_SRC_DIR:-src}"
CPPCHECK="${CPPCHECK:-cppcheck}"

SCL_UTILITY_SRC="${SCL_UTILITY_SRC:-}"
if [ -z "$SCL_UTILITY_SRC" ]; then
    if   [ -d ../utility/src ];       then SCL_UTILITY_SRC=../utility/src
    elif [ -d /tmp/scl-utility/src ]; then SCL_UTILITY_SRC=/tmp/scl-utility/src
    fi
fi
[ -n "$SCL_UTILITY_SRC" ] || { echo "scl-utility src not found; set SCL_UTILITY_SRC" >&2; exit 1; }

[ -d "$SCL_SRC_DIR" ] || { echo "No sources ($SCL_SRC_DIR), skipping."; exit 0; }

find "$SCL_SRC_DIR" \( -name '*.h' -o -name '*.hpp' \) -exec "$CPPCHECK" \
    --enable=warning,style,performance,portability \
    --std=c++20 \
    --language=c++ \
    --inline-suppr \
    --error-exitcode=1 \
    --suppressions-list=.cppcheck \
    -I"$SCL_SRC_DIR" \
    -I"$SCL_UTILITY_SRC" \
    -UDOXYGEN \
    {} +
