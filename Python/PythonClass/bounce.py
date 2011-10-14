def printTable(height):
	for i in range(10):
		height = height*.6
		print "%i %1f" % ((i+1), height) 
		

printTable(100)