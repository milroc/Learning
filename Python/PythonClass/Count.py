#class Count:
#    def __init__(self, n):
#        self.n = n
#        self.i = 0
#    def next(self):
#        if self.i < self.n:
#            value = self.i
#            self.i+=1
#            return value
#        raise StopIteration
#    def __iter__(self):
#        return self

def count(n):
    for i in range(n):
        yield i

if __name__ == '__main__':
#    for i in Count(10):
#        print i
#    print
    c = count(10)
    dir(c)
    