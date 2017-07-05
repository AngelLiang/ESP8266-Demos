# codeing=utf-8
# Python 3

import socket, ssl
import time

# cacrtf = "ca/ca.crt"
# crtf = "ca/server.crt"
# keyf = "ca/server.key"

# CA 证书
cacrtf = "TLS_BiDirectVerif_Demo/TLS.ca_x509.pem"
crtf = "TLS_BiDirectVerif_Demo/TLS.ca_x509.pem"
keyf = "TLS_BiDirectVerif_Demo/TLS.ca_key.pem"


server_sc = socket.socket()
server_sc.bind(('', 2000))
server_sc.listen(5)

newsocket, addr = server_sc.accept()
sc = ssl.wrap_socket(newsocket,
                     server_side=True,
                     certfile=crtf,
                     keyfile=keyf,
                     ca_certs=cacrtf
                     )

data = sc.read()
print(data)
sendData = "Server Got the request!"
sc.write(bytes(sendData, encoding="utf-8"))

sc.close()
server_sc.close()
