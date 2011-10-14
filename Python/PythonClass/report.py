# report.py
import fieldparse

def read_portfolio(filename):
    return fieldparse.parse(open(filename),[str,int,float],['name','shares','price'])


def portfolio_total(portfolio):
    total = 0.0
    for name, shares, price in portfolio:
        total += shares*price
    return total

def read_prices(filename):
    return dict(fieldparse.parse(open(filename),[str,float],sep=','))

def make_report(prices, portfolio):
    ret = []
    for s in portfolio:
        ret.append((s['name'], s['shares'], 
                    prices[s['name']], prices[s['name']] - s['price']))
    return ret

def pretty_print():
    prices = read_prices("Data/prices.csv")
    portfolio = read_portfolio("Data/portfolio.dat")
    report = make_report(prices, portfolio)
    headers = ('Name', 'Shares', 'Price', 'Change')
    print "%10s %10s %10s %10s" % headers
    
    for r in report:
        print "%10s %10d %10.2f %10.2f" % r    
        
    cost = sum([s['shares']*s['price'] for s in portfolio])
    value = sum([s['shares']*prices[s['name']] for s in portfolio])
    more100 = [s for s in portfolio if s['shares'] > 100]
    print "Cost %d Value %d" % (cost, value)
    
    print more100
    
    summary = [(s['name'],s['shares'],prices[s['name']]) for s in portfolio]
    for s in summary: print s

def print_report(report):
    headers = ('Name', 'Shares', 'Price', 'Change')
    print "%10s " * len(headers) % headers
    for r in report:
        print "%10s %10d %10.2f %10.2f" % r

def portfolio_report(portfolio, prices):
    prices = read_prices(prices)
    portfolio = read_portfolio(portfolio)
    report = make_report(prices, portfolio)
    print_report(report)

if __name__ == '__main__':
    portfolio_report("Data/portfolio.dat","Data/prices.csv")

