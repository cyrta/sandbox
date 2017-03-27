import json
import requests

BASE_URL = 'http://localhost/wiki/' 
AUTH_USER = 'user'
AUTH_PASS = 'password123!'
POST_ID = 1234

def update_wiki():

    ss = requests.Session()

    ss.post(BASE_URL + '/dologin.action', data = { 'os_username': AUTH_USER, 'os_password': AUTH_PASS, 'login': 'Log+in', 'os_destination': '' })

    metadata = ss.get('{}/rest/api/content/{}'.format(BASE_URL, POST_ID)).json()

    post_title = metadata['title']
    post_space = metadata['space']['key']
    post_version = metadata['version']['number']

    content = ss.get('{}/rest/api/content/{}?expand=body.storage'.format(BASE_URL, POST_ID)).json()

    post_content = content['body']['storage']['value']

	#
	# Do stuff here with post_content
	#
	
    update_body = {
        'id': POST_ID,
        'type': 'page',
        'title': post_title,
        'space': {
            'key': post_space
        },
        'body': {
            'storage': {
                'value': post_content,
                'representation': 'storage'
            }
        },
        'version': {
            'number': int(post_version) + 1
        }
    }

    resp = ss.put('{}/rest/api/content/{}'.format(BASE_URL, POST_ID), json=update_body)
    if resp.status_code == 200:
        print 'OK'
    else:
        print resp.content