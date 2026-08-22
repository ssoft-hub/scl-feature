#!/usr/bin/env bash
#
# clang-tidy lint — single source of truth for CI and local (IDE) runs.
# Mirrors the clang-tidy job in .gitlab-ci.yml / .github/workflows/lint.yml.
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
#   CLANG_TIDY       clang-tidy executable (default: clang-tidy on PATH)
#   SCL_LINT_JOBS    headers to scan at once (default: one per core)
#
set -euo pipefail
cd "$(dirname "$0")/../.."

SCL_SRC_DIR="${SCL_SRC_DIR:-src}"
CLANG_TIDY="${CLANG_TIDY:-clang-tidy}"
SCL_LINT_JOBS="${SCL_LINT_JOBS:-$(getconf _NPROCESSORS_ONLN 2> /dev/null || echo 2)}"

SCL_UTILITY_SRC="${SCL_UTILITY_SRC:-}"
if [ -z "$SCL_UTILITY_SRC" ]; then
    if   [ -d ../utility/src ];       then SCL_UTILITY_SRC=../utility/src
    elif [ -d /tmp/scl-utility/src ]; then SCL_UTILITY_SRC=/tmp/scl-utility/src
    fi
fi
[ -n "$SCL_UTILITY_SRC" ] || { echo "scl-utility src not found; set SCL_UTILITY_SRC" >&2; exit 1; }

[ -d "$SCL_SRC_DIR" ] || { echo "No sources ($SCL_SRC_DIR), skipping."; exit 0; }

# A header is a whole translation unit to clang-tidy, and they share nothing, so the run
# costs one core's time divided by the cores there are. Findings of two headers can
# interleave; every diagnostic line carries the file it belongs to.
find "$SCL_SRC_DIR" \( -name '*.h' -o -name '*.hpp' \) -print0 |
    xargs -0 -r -P "$SCL_LINT_JOBS" -I{} \
        "$CLANG_TIDY" {} --quiet --warnings-as-errors='*' -- \
        -std=c++20 -xc++ -I"$SCL_SRC_DIR" -I"$SCL_UTILITY_SRC" || exit 1
