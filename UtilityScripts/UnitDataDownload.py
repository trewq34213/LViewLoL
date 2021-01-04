import sys, urllib.request, re, time, os
from pprint import pprint

result_folder = 'unit_data'
pattern_unit = '<a href="[\w]+/" title="[\w]+">([\w]+)/</a>'
pattern_url_list = 'https://raw.communitydragon.org/{}/game/data/characters/'
pattern_url_unit_data = 'https://raw.communitydragon.org/{}/game/data/characters/{}/{}.bin.json'

version = sys.argv[1]
url = pattern_url_list.format(version)

if not os.path.isdir(result_folder):
	os.mkdir(result_folder)

print('Requesting: ' +  url)
headers = {}
headers['User-Agent'] = "Mozilla/5.0 (X11; Linux i686) AppleWebKit/537.17 (KHTML, like Gecko) Chrome/24.0.1312.27 Safari/537.17"
req = urllib.request.Request(url, headers = headers)
page = urllib.request.urlopen(req).read().decode('utf-8')

failed = []
matches = re.findall(pattern_unit, page)
for match in matches:
	url = pattern_url_unit_data.format(version, match, match)
	print('Requesting: ' + url)
	
	try:
		req = urllib.request.Request(url, headers = headers)
		json = urllib.request.urlopen(req).read().decode('ascii')
		with open(os.path.join(result_folder, match), 'w') as file:
			file.write(json)
	except Exception as e:
		print('Failed to retrieve data for {}. ({})'.format(match, str(e)))
		failed.append(match)
	time.sleep(0.1)
	
print('Error retrieving following units:')
pprint(failed)