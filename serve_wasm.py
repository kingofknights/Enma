#!/usr/bin/env python3
import http.server
import socketserver
import os
import sys

PORT = 8080
DIRECTORY = "build_wasm"

if not os.path.exists(DIRECTORY):
    print(f"Error: Directory '{DIRECTORY}' not found.")
    print("Please run ./build_wasm.sh first.")
    sys.exit(1)

class Handler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=DIRECTORY, **kwargs)

    def end_headers(self):
        # Adding these headers is helpful if you ever start using multi-threading
        # in your WebAssembly app (SharedArrayBuffer requirements)
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        super().end_headers()

# Explicitly ensure the .wasm file extension maps to the correct MIME type
Handler.extensions_map['.wasm'] = 'application/wasm'

with socketserver.TCPServer(("", PORT), Handler) as httpd:
    print(f"Serving WebAssembly build at http://localhost:{PORT}")
    print("Press Ctrl+C to stop.")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nStopping server.")
