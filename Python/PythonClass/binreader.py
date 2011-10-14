import struct

record_fmt = "<8sif"

record_size = struct.calcsize(record_fmt)

def read_portfolio(portfolio):
    f = open(portfolio, "rb")
    stockList = []
    while True:
        rawrecord = f.read(record_size)
        if not rawrecord:
            break
        name,shares,price = struct.unpack(record_fmt, rawrecord)
        
        stock = {
             'name'   : name.strip('\x00'), #why
             'shares' : shares,
             'price'  : price
             }
        stockList.append(stock)
    f.close()
    return stockList

if __name__ == '__main__':
    portfolio = read_portfolio("Data/portfolio.bin")
    for p in portfolio:
        print p