#rivers.py
import BeautifulSoup
import urllib

if __name__ == '__main__':
    soup = BeautifulSoup.BeautifulSoup(urllib.urlopen("http://bit.ly/py_rivers_ex"))
    table = soup.find('table', border='1', bordercolor='#000000')
    for row in table.findAll('tr'):
        print [str(col.text) for col in row.findAll('td')]
    