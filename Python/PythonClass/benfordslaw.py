import re
import urllib

def benfordsLaw(file):
    nums = re.findall('\b([1-9]+[.,0-9]*)\b', file.read())
    firstDigits = [num[0] for num in nums]
    return firstDigits

url = 'http://www.worldatlas.com/citypops.htm'
bl = benfordsLaw(urllib.urlopen(url))
print bl

#for b in bl:
#   print b