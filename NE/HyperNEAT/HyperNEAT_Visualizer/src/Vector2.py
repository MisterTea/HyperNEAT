import collections

class Vector2(object):
    __slots__ = ('x','y')
    
    def __init__(self,x=0,y=0):
        self.x = x
        self.y = y
    
    def __add__(self,other):
        return Index3(self.x+other.x,self.y+other.y)
        
    def __sub__(self,other):
        return Index3(self.x-other.x,self.y-other.y)

    def __mul__(self,scalar):
        return Index3(self.x*scalar,self.y*scalar)

    def __div__(self,scalar):
        return Index3(self.x/scalar,self.y/scalar)

    def maxDimension(self):
        return max(self.x,self.y)

    def minDimension(self):
        return min(self.x,self.y)
        
    def chessDistance(self,p2):
        return max(abs(self.x-p2.x),abs(self.y-p2.y))

    def manhatDistance(self,p2):
        return abs(self.x-p2.x)+abs(self.y-p2.y)

        