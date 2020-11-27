# C++ Initialization Flowchart

This repository contains code to generate a flowchart showing the initialization rules for C++20.

The generated files are hosted on Github Pages: [SVG](https://randomnetcat.github.io/cpp_initialization/initialization.svg), [PNG](https://randomnetcat.github.io/cpp_initialization/initialization.png).

## Building

The repository contains an M4 file which can be preprocessed into a graphviz dot file using GNU M4. Feed the preprocessed file into dot to get an image file.

Example for SVG: `m4 flowchart.dot.m4 | dot -Tsvg > flowchart.svg`

Example for PNG: `m4 flowchart.dot.m4 | dot -Tpng > flowchart.png`

Consult the `dot` manual for all of the file types and other options for controlling `dot`.
