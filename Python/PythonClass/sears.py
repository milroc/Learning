# sears.py
bill_thickness = 0.11 * 0.001 # Meters (0.11 millimeter)
sears_height   = 442          # Meters

# Initial condition (a single bill)
day = 1
num_bills = 1
stack_height = num_bills * bill_thickness

# Add bills and print a table
while stack_height < sears_height:
    print day, num_bills, stack_height
    day = day + 1
    num_bills = num_bills + 2*num_bills
    stack_height = num_bills * bill_thickness

print "Number of days", day
print "Number of bills", num_bills
print "Final height", stack_height