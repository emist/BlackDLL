import nasty
import inspect

def writeout(filename, module):
	f = open("C:\\Users\\emist\\" + filename + ".txt", 'w')
	for x in inspect.getmembers(module):
		f.write(x[0])
		f.write('\n')
	f.close()

		
	