import re
import requests
import os.path
from bs4 import BeautifulSoup
import util


class Cart(object):
    ''' Define all the attributes of a cart '''

    def __init__(self, tid):
        self._tid = tid

        # Open the cart page for parsing
        soup = self._open_cart_page()

        # Parse the title of the cart
        self._title = soup.title.contents

        # Parse the number of the cart
        number_label = str(soup.find(string=re.compile('#[0-9]+#')))
        self._number = int(number_label[2:-2])

    def _open_cart_page(self):
        ''' Return a BeautifulSoup object for the desired cart's page '''
        html = requests.get(util.BBS_BASE_URL + '?tid=' + str(self._tid)).text
        return BeautifulSoup(html, 'html.parser')

    def _cart_data(self):
        ''' Retrieve the byte data of this cartridge '''
        data = requests.get(util.BBS_BASE_URL + 'cposts/1/' +
                            str(self._number) + '.p8.png').content
        return data

    def _filename(self):
        ''' Convert the title of this cartridge into a cart filename '''
        name = self._title.lower().replace(' ', '-') + '.p8.png'
        return name

    def download(self):
        ''' Download this cartridge as a named .p8.png file '''
        file = open(self._filename(), 'wb')
        data = self._cart_data()
        file.write(data)
        file.close()

    def is_downloaded(self):
        return os.path.isfile(self._filename())
