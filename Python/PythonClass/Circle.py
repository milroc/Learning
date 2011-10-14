import math

class Circle(object):
    'The Might Circle!'
    def __init__(self,radius):
        self.diam = radius * 2
        
    def area(self):
        'how much inside'
        return (self.diam/2)**2 * 3.14159
    
    def perimeter(self):
        'how much around the outside'
        return (self.diam/2) * 2 * 3.14159
    
    @property
    def radius(self):
        return self.diam / 2
    
    @staticmethod
    def compute_area(radius):
        'stand-alone area() computator'
        return radius ** 2 * 3.14159
    
    @classmethod
    def from_diameter(cls, diameter):
        'alternate constructor'
        return cls(diameter/2)
    
    
