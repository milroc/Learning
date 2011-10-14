import random

class Bloom:
    def __init__(self, n = 56, r = 34):
        self.n = n 
        self.r = r 
        self.arr = [0] * n
    def add(self, key):
        random.seed(key)
        lucky = random.sample(xrange(self.n), self.r)
        for num in lucky:
            self.arr[num] = 1
    def __contains__(self, key):
        random.seed(key)
        lucky = random.sample(xrange(self.n), self.r)
        for num in lucky:
            #self.arr[num] != 1
            pass