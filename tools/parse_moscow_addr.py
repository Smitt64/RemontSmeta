from selenium import webdriver
from selenium.webdriver.chrome.options import Options
import json
from lxml import html
import os

URL = u'https://ru.wikipedia.org/wiki/%D0%A1%D0%BF%D0%B8%D1%81%D0%BE%D0%BA_%D1%83%D0%BB%D0%B8%D1%86_%D0%9C%D0%BE%D1%81%D0%BA%D0%B2%D1%8B'

opts = Options()
opts.binary_location = 'E:\\Bruce\\Programming\\RemontSmeta\\tools'#'chromedriver.exe'
driver = webdriver.Chrome('chromedriver.exe')
driver.get(URL)
#print(driver.page_source)
'''page = requests.get(URL)

print(page.text)'''

tree = html.fromstring(driver.page_source)
columns = tree.xpath('//div[@class="columns"]/ul/li/a/text()')

print(columns)

data = {}
data['streets'] = columns
with open('msc_streets.json', 'w', encoding='utf8') as outfile:
    outfile.write(json.dumps(data, indent=4, sort_keys=True, ensure_ascii=False))