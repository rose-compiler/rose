#!/bin/bash
# autogen.sh - Generate configure script and Makefiles

set -e

echo "Generating build system files..."

# Run autoreconf to generate configure and Makefile.in
autoreconf --install --verbose --force

echo ""
echo "Build system generated successfully!"
echo ""
echo "Next steps:"
echo "  1. Set PKG_CONFIG_PATH: export PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig:\$PKG_CONFIG_PATH"
echo "  2. Configure:           ./configure"
echo "  3. Build:               make"
echo "  4. Run:                 ./simple_analyzer test_input.c"
echo ""
