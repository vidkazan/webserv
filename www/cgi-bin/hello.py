import os

print("Content-type: text/html")
print()
print("<h1>CGI env:</h1>")

for param in os.environ.keys():
	print("<br>%20s</br>: %s<br>" % (param, os.environ[param]))