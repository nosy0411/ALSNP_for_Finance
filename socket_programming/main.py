import socket
import msvcrt

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(('127.0.0.1',20002))

while True:
    data, addr = sock.recvfrom(1024)
    print(data.decode())

sock.close()