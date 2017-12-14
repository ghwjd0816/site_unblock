import socket
import SocketServer

def recv_until(sock):
	t = ''
	find = '\r\n\r\n'
	while True:
		m=sock.recv(1)
		if len(m)==0:
			return 0
		t+=m
		if find in t:
			return t

def findhost(data):

	index = data.find("Host: ")
	if index<0:
		return 0
	host = data[index:].split("\r\n",1)[0]
	return host[6:]
	
def recv_num(sock, now):
	buf = ""
	while len(buf)<now:
		buf+=sock.recv(1)

	return buf

def recv_content(sock, header):
	data=""
	index = header.find("Content-Length: ")
	if index<0 :
		index = header.find("Transfer-Encoding: chunked")
		if index<0 :
			return ""
		while True:
			num = recv_until(sock)[:-4]
			num = int(num)
			if num==0:
				return data
			data += recv_num(sock, num)
	contentlength = int(header[index+16:].split("\r\n",1)[0])
	print(contentlength)
	return recv_num(sock,contentlength)


class handler(SocketServer.BaseRequestHandler):

	def handle(self):
		data = recv_until(self.request)
		if data == 0:
			print("[-]Failed to receive data")
			return

		print(data)

		host = findhost(data)
		if not host:
			print("[-]Failed to find host")

		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sock.connect((host,80))

		dummy = "Get http://www.dummydata.com/ HTTP/1.1\r\nHost: www.dummydata.com\r\n\r\n"

		sock.send(dummy + data)
		print("send OK")

		dummyheader = recv_until(sock)
		content = recv_content(sock,dummyheader)
		print("dummyheader : \n"+dummyheader)
		print("duumycontents : \n"+content)
		header = recv_until(sock)
		content = recv_content(sock, header)
		print("real : "+header+content)

		self.request.sendall(header + content)

if __name__ == "__main__":

	host, port = "localhost",8080

	#instantiate the server, and bind to localhost on port 8080
	server = SocketServer.TCPServer((host,port), handler)
	#activate the server
	server.serve_forever()
