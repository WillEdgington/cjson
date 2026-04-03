# cjson

A JSON lexer and parser written in C from scratch, built as part of a [self-directed C curriculum](https://github.com/WillEdgington/c-curriculum).

Supports strings, numbers, booleans, null, nested objects, and arrays. Parses input into an AST, pretty-prints it back to JSON, and supports path-based querying of the tree.

---

## Build

Requires `gcc` and `make`

```bash
make
```

To remove compiled objects, dependency files, and the binary:

```bash
make clean
```

### Compiler flags

|Flag|Purpose|
|---|---|
|`-Isrc`|Tells the compiler to look in `src/` for header files|
|`-Wall -Wextra`|Enables a broad set of compiler warnings|
|`-pedantic`|Enforces strict ISO C compliance|
|`fsanitize=address`|AddressSanitizer: detects memory errors such as use-after-free and buffer overflows at runtime|
|`-fsanitize=undefined`|UndefinedBehaviorSanitizer: detects undefined behaviour such as signed integer overflow and null pointer dereference at runtime|
|`-MMD -MP`|Generates `.d` dependency files so `make` recompiles only what has changed when a header is modified|

The sanitizer flags are intended for development builds. Strip them for release by removing `-fsanitize=address,undefined` from both `CFLAGS` and `LDFLAGS`.

---

## Usage

Print the entire JSON tree:

```bash
./cjson <file.json>
```

Query a specific path:

```bash
./cjson <file.json> <path>
```

Paths are dot-separated. Numeric segments index into arrays.

### Examples

```bash
./cjson tests/test.json
```

```json
[
  {
    "this": [
      1,
      4,
      5,
      null,
      false,
      {
        "hello": "there"
      }
    ],
    "is": 100.453
  },
  [
    "a",
    "test",
    true
  ]
]
```

```bash
./cjson tests/test.json "0.this"
```

```json
[
  1,
  4,
  5,
  null,
  false,
  {
    "hello": "there"
  }
]
```

```bash
./cjson tests/test.json "0.this.5.hello"
```

```json
"there"
```

---

## Project structure
```
src/
├── main.c        entry point, reads file, drives the pipeline, handles CLI args
├── read.c/h      reads a file into a heap-allocated null-terminated string
├── lexer.c/h     tokenises raw JSON text into a flat token sequence
├── parser.c/h    builds a JsonNode AST from the token sequence
├── json.c/h      defines JsonNode types, pretty-printing, and memory freeing
└── query.c/h     path-based and index-based queries on a JsonNode tree
tests/
├── test.json                         well-formed JSON for testing correct output
├── bad_json.json                     malformed JSON for testing error handling
└── predictor_model_definition.json   real-world nested JSON from a deep learning project
```

---

## Architecture

The pipeline has four stages, with an optional query step:

**File reading:** `read_file` in `read.c` loads the input file into a heap-allocated null-terminated string using `fseek`/`ftell`/`fread`. The caller owns the buffer and frees it after parsing.

**Lexer:** scans the raw input string character by character and produces tokens one at a time (`LBRACE`, `STRING`, `NUMBER`, `JSON_TRUE` etc.). The parser calls `lexer_next_token` to advance through the token stream.

**Parser:** uses recursive descent to consume tokens and build a tree of `JsonNode` structs. Each JSON value type has its own parse function (`parse_object`, `parse_array`, `parse_string` etc.) that calls `parse_value` recursively for nested structures.

**JsonNode tree:** the in-memory representation of the parsed document. Each node carries a type tag and a union value. The tree is walked by `json_print` to produce formatted output and freed recursively by `json_free`.

**Query:** `query.c` exposes read-only traversal functions that operate on the JsonNode tree. `json_get_key` and `json_get_index` perform single-step lookups on objects and arrays, respectively. `json_get_path` composes these into dot-separated path traversal. At each segment it inspects the current node's type and dispatches accordingly. If the node is an array and the segment is numeric, it indexes into the array. If the node is an object, it performs a key lookup regardless of whether the segment is numeric or not. `json_has_key` checks for key existence in an object without returning the value.

---

## Error handling

If the input is malformed, the parser returns `NULL`, all partially allocated nodes are freed, and the program exits with a parse error message. Memory correctness is verified with AddressSanitizer and Valgrind.

---

## Author

Created by [**WillEdgington**](https://github.com/WillEdgington)

📧 [**willedge037@gmail.com**](mailto:willedge037@gmail.com) &nbsp;|&nbsp; 🔗 [**LinkedIn**](https://www.linkedin.com/in/williamedgington/)
