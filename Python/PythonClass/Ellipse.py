import math
import Circle

class Ellipse(Circle):
    def __init__(self, radius, viewangle):
        self.viewangle = viewangle
#        Circle.__init__(self, radius)
        super(Ellipse, self).__init__(radius)
        #super is able to interpret multiple inheritance objects 
        #read a Blog post by Raymond Hettinger
 
    @property
    def apparent_area(self):
        return self.area() * math.sin(self.viewangle)