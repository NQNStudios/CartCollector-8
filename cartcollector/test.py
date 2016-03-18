import re
import requests
from bs4 import BeautifulSoup

i = 99999999

url = 'http://www.lexaloffle.com/bbs/?tid=' + str(i)

html = requests.get(url).text
soup = BeautifulSoup(html, 'html.parser')

links = soup.find_all('a')

print(i)

if re.search(re.compile('cat=7.+sub=2'), str(links[7])):
    print True
