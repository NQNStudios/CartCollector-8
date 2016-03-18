import re
import requests
from bs4 import BeautifulSoup

BBS_ROOT_URL = 'http://www.lexaloffle.com/bbs/'
FIRST_CART_TID = 1808 # Determined once
REASONABLE_STOPPING_POINT = 30


class CartFinder(object):
    ''' Brute forces the 'tid' parameter to find PICO-8 cartridges '''

    def __init__(self, cat=7, sub=2):
        self._cat = cat
        self._sub = sub

    def bbs_url(self, tid):
        ''' Construct the URL of a BBS submission from its tid '''
        return BBS_ROOT_URL + '?tid=' + str(tid)

    def bbs_soup(self, tid):
        ''' Retrieve a BeautifulSoup object of the page for the given BBS
        submission
        '''
        url = self.bbs_url(tid)
        html = requests.get(url).text
        soup = BeautifulSoup(html, 'html.parser')
        return soup

    def is_cart(self, soup):
        ''' Check if the BBS submission is a desired cartridge '''
        links = soup.find_all('a')

        identifier_regex = re.compile(
            'cat=' + str(self._cat) + '.+sub=' + str(self._sub))

        return re.search(identifier_regex, links[7])

    def FindCarts(self):
        ''' Return a p8.png link to each of the carts found '''
        tid = FIRST_CART_TID
        consecutive_fails = 0
        cart_urls = []

        while consecutive_fails < REASONABLE_STOPPING_POINT:
            soup = self.bbs_soup(tid)

            success = self.is_cart(soup)

            if success:
                cart_number_label = soup.find(string=re.compile('#[0-9]+#'))
                cart_number = int.parse(cart_number_label[2:-2])
                cart_url = BBS_ROOT_URL + 'cposts/0/' + str(cart_number) + '.p8.png'
                cart_urls.append(cart_url)
            else:
                consecutive_fails += 1

        return cart_urls
