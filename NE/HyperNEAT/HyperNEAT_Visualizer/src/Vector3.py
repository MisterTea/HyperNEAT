import collections

class Vector3(object):
    __slots__ = ('x','y','z')
    
    def __init__(self,x=0,y=0,z=0):
        self.x = x
        self.y = y
        self.z = z
    
    def __add__(self,other):
        return Index3(self.x+other.x,self.y+other.y,self.z+other.z)
        
    def __sub__(self,other):
        return Index3(self.x-other.x,self.y-other.y,self.z-other.z)

    def __mul__(self,scalar):
        return Index3(self.x*scalar,self.y*scalar,self.z*scalar)

    def __div__(self,scalar):
        return Index3(self.x/scalar,self.y/scalar,self.z/scalar)

    def maxDimension(self):
        return max(self.x,self.y,self.z)

    def minDimension(self):
        return min(self.x,self.y,self.z)
        
    def chessDistance(self,p2):
        return max(abs(self.x-p2.x),abs(self.y-p2.y),abs(self.z-p2.z))

    def manhatDistance(self,p2):
        return abs(self.x-p2.x)+abs(self.y-p2.y)+abs(self.z-p2.z)

        