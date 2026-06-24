#!/bin/sh
# Build versioned Doxygen docs for this module.
# Must be run from the module root directory.
#
# Usage: sh script/doxygen/build.sh [output_dir]
#   output_dir — defaults to doc/doxygen/

set -e

OUT="${1:-doc/doxygen}"
DOXYFILE="project/doxygen/Doxyfile"

PROJECT_NAME=$(grep '^PROJECT_NAME\s*=' "${DOXYFILE}" | sed 's/.*=\s*//' | sed 's/\s*$//')
DISPLAY_NAME=$(echo "${PROJECT_NAME}" | tr '.' ' ')

native_path() {
  if command -v cygpath >/dev/null 2>&1; then cygpath -w "$1"; else echo "$1"; fi
}

# On Windows, python3 may resolve to a non-executable Store stub; prefer python.
if python3 -c "" 2>/dev/null; then PYTHON=python3; else PYTHON=python; fi

git fetch --prune --prune-tags origin 2>/dev/null || true
TAGS=$(git tag -l 'v[0-9]*' 2>/dev/null | sort -V || true)

DEV_REF=""
if git rev-parse --verify dev >/dev/null 2>&1; then
  DEV_REF="dev"
elif git rev-parse --verify "origin/dev" >/dev/null 2>&1; then
  DEV_REF="origin/dev"
fi

VERSIONS_JSON=$($PYTHON - "${TAGS}" "${DEV_REF}" << 'EOF'
import sys
tags_raw, dev_ref = sys.argv[1], sys.argv[2]
tags = [t for t in tags_raw.split('\n') if t.strip()]
items = []
if dev_ref:
    items.append('{"v":"dev","label":"dev (latest)"}')
for t in reversed(tags):
    items.append('{"v":"%s","label":"%s"}' % (t, t))
print('[' + ','.join(items) + ']')
EOF
)

ORIG_HEAD=$(git rev-parse HEAD)
BUILD_TMP=$(mktemp -d)
cleanup() { rm -rf "${BUILD_TMP}"; git checkout -f "${ORIG_HEAD}" 2>/dev/null || true; }
trap cleanup EXIT

mkdir -p "${OUT}"

build_version() {
  VERSION="$1"
  VER_OUT="${OUT}/${VERSION}"
  rm -rf "${VER_OUT}"
  mkdir -p "${VER_OUT}"

  _git_dir=$(git rev-parse --absolute-git-dir 2>/dev/null || true)
  _git_work=$(git rev-parse --show-toplevel 2>/dev/null || true)

  (cat "${DOXYFILE}";
   printf '\nGENERATE_HTML=YES\nGENERATE_XML=NO\nHTML_OUTPUT=.\nOUTPUT_DIRECTORY=%s\n' \
     "$(native_path "${VER_OUT}")") | \
    GIT_DIR="${_git_dir}" GIT_WORK_TREE="${_git_work}" doxygen -

  $PYTHON - "${VER_OUT}" "${DISPLAY_NAME}" "${VERSION}" "${VERSIONS_JSON}" << 'EOF'
import os, sys
out_dir, display_name, version, versions_json = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4]
nav = (
    '<div id="scl-nav" style="background:#1565c0;color:#fff;padding:8px 16px;'
    'font-family:sans-serif;display:flex;align-items:center;gap:12px;font-size:14px">'
    '<span style="font-weight:bold">' + display_name + '</span>'
    '<span style="opacity:.5">/</span>'
    '<select id="scl-ver" style="background:#0d47a1;color:#fff;'
    'border:1px solid rgba(255,255,255,.4);padding:3px 8px;cursor:pointer;font-size:13px">'
    '</select></div>\n'
    '<script>(function(){'
    'var V=' + versions_json + ';'
    'var cur="' + version + '";'
    'var s=document.getElementById("scl-ver");'
    'V.forEach(function(v){'
    'var o=document.createElement("option");'
    'o.value=v.v;o.text=v.label;'
    'if(v.v===cur)o.selected=true;'
    's.appendChild(o);});'
    's.onchange=function(){window.location="../"+s.value+"/";};'
    '})();</script>\n'
)
for root, dirs, files in os.walk(out_dir):
    for fname in files:
        if not fname.endswith('.html'):
            continue
        path = os.path.join(root, fname)
        try:
            content = open(path, encoding='utf-8', errors='ignore').read()
            idx = content.find('<body')
            if idx < 0:
                continue
            idx = content.find('>', idx) + 1
            open(path, 'w', encoding='utf-8').write(content[:idx] + nav + content[idx:])
        except Exception:
            pass
EOF
}

build_index() {
  $PYTHON - "${OUT}/index.html" "${DISPLAY_NAME}" "${TAGS}" "${DEV_REF}" << 'EOF'
import sys
idx_file, display_name, tags_raw, dev_ref = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4]
tags = [t for t in tags_raw.split('\n') if t.strip()]
items = ''
if dev_ref:
    items += '<li><a href="dev/">dev <em>(latest)</em></a></li>\n'
for t in reversed(tags):
    items += '<li><a href="%s/">%s</a></li>\n' % (t, t)
redirect = 'dev' if dev_ref else (list(reversed(tags))[0] if tags else '')
html = '''<!DOCTYPE html>
<html><head><meta charset="utf-8">
<meta http-equiv="refresh" content="0; url={r}/">
<title>{n} - Versions</title>
<style>body{{font-family:sans-serif;max-width:600px;margin:2em auto}}</style>
</head><body><h1>{n}</h1><ul>{items}</ul>
</body></html>'''.format(n=display_name, r=redirect, items=items)
open(idx_file, 'w').write(html)
EOF
}

echo "=== ${DISPLAY_NAME}: building docs ==="
echo "    Tags: $(echo "$TAGS" | tr '\n' ' ')"

for TAG in $TAGS; do
  echo "  [build] ${TAG}"
  git checkout -f "${TAG}"
  build_version "${TAG}"
done

if [ -n "$DEV_REF" ]; then
  echo "  [build] dev"
  git checkout -f "${DEV_REF}"
  build_version "dev"
fi

git checkout -f "${ORIG_HEAD}"
build_index
echo "=== Done: ${OUT} ==="
