GENERATED_FILE="generated-flowchart.dot"

m4 flowchart.dot.m4 > "$GENERATED_FILE"
cat "$GENERATED_FILE" | dot -Tsvg > rendered.svg
