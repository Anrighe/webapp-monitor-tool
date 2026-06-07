from http.server import BaseHTTPRequestHandler, HTTPServer

class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        print(f"Received request: {self.path}")

        if self.path == '/q/health/ready':
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            self.wfile.write(b'{"status":"ok"}')
        else:
            self.send_response(404)
            self.end_headers()
            self.wfile.write(b'{"status":"not found"}')

server = HTTPServer(('0.0.0.0', 8080), Handler)
print('Listening on http://localhost:8080')
server.serve_forever()