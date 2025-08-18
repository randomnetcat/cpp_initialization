#!/usr/bin/env bash

set -eu -o pipefail

cd_source_dir() {
	cd -- "$(dirname -- "${BASH_SOURCE[0]}")" > /dev/null 2>&1
}

(cd_source_dir; cat flowchart.dot.cpp | gcc -E - | ./wrap.perl 70 | dot -Tsvg) > "$1"
(cd_source_dir; cat flowchart.dot.cpp | gcc -E - | ./wrap.perl 70 | dot -Tpng) > "$2"
