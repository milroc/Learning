principal = 500000.00    # Mortgage amount
rate = 0.04              # Interest rate
payment = 499.00            # Monthly payment
i = 0
total_paid = 0.00
while (0 < principal):
	principal = principal*(1+rate/12) - payment
	i+=1
	total_paid += payment
	if (i == 24): 
		rate = .09 
		payment = 3999.00
	print "%i %i %f" % (i, total_paid, principal)	

print "Total Months ", i
print "Total paid ", principal