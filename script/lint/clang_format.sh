#!/usr/bin/env bash
#
# clang-format lint — single source of truth for CI and local (IDE) runs.
# Mirrors the clang-format job in .gitlab-ci.yml / .github/workflows/lint.yml.
#
# Env overrides:
#   SCL_SRC_DIR    source directory to scan (default: src)
#   CLANG_FORMAT   clang-format executable (default: clang-format on PATH)
#
set -euo pipefail
cd "$(dirname "$0")/../.."

SCL_SRC_DIR="${SCL_SRC_DIR:-src}"
CLANG_FORMAT="${CLANG_FORMAT:-clang-format}"

[ -d "$SCL_SRC_DIR" ] || { echo "No sources ($SCL_SRC_DIR), skipping."; exit 0; }

find "$SCL_SRC_DIR" \( -name '*.h' -o -name '*.hpp' \) \
    | xargs --no-run-if-empty "$CLANG_FORMAT" --dry-run --Werror
