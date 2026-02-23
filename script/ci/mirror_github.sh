#!/bin/sh
# Push an exact mirror of all branches and tags to GitHub.
#
# Required variables:
#   GITHUB_MIRROR_URL  - git@github.com:org/repo.git  (manual, SSH)
#                      - https://github.com/org/repo.git  (CI, HTTPS)
#   GITHUB_TOKEN       - GitHub PAT, required only for HTTPS

set -e

if [ -z "$GITHUB_MIRROR_URL" ]; then
    echo 'GITHUB_MIRROR_URL is not set' >&2
    exit 1
fi

if [ -n "$GITHUB_TOKEN" ]; then
    case "$GITHUB_MIRROR_URL" in
        https://*) GITHUB_AUTH_URL="https://oauth2:${GITHUB_TOKEN}@${GITHUB_MIRROR_URL#https://}" ;;
        *) echo 'GITHUB_TOKEN is set but GITHUB_MIRROR_URL is not an HTTPS URL' >&2; exit 1 ;;
    esac
else
    GITHUB_AUTH_URL="$GITHUB_MIRROR_URL"
fi

cleanup() {
    git remote remove github 2>/dev/null || true
}
trap cleanup EXIT

git fetch --all --tags --prune
git remote remove github 2>/dev/null || true
git remote add github "$GITHUB_AUTH_URL"
git push github 'refs/remotes/origin/*:refs/heads/*' --force --prune
git push github ':refs/heads/HEAD' 2>/dev/null || true
git push github --tags --prune
