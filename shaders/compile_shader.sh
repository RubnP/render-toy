#!/bin/bash

# Check if a file was passed
if [ $# -ne 1 ]; then
  echo "Usage: $0 <shader-file.glsl>"
  exit 1
fi

SHADER_FILE="$1"

# Check if the file exists
if [ ! -f "$SHADER_FILE" ]; then
  echo "Error: File '$SHADER_FILE' not found."
  exit 1
fi

# Output: same filename with .spv appended
OUTPUT="${SHADER_FILE}.spv"

# Run glslc
echo "Compiling $SHADER_FILE -> $OUTPUT"
glslc "$SHADER_FILE" -o "$OUTPUT"

# Check if compilation succeeded
if [ $? -eq 0 ]; then
  echo "Compilation successful."
else
  echo "Compilation failed."
  exit 1
fi
