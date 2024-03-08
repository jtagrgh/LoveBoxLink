#!/usr/bin/env python3

import os
import subprocess
import json


def make_secrets_file():
	secrets_file_path = 'src/Client/secrets.h'

	if (os.path.isfile(secrets_file_path)):
		should_overwrite = ''

		while (should_overwrite not in ('y', 'n')):
			should_overwrite = input('Secrets file already exists. Would you like to overwrite it? [y/n]: ')
			if (should_overwrite == 'n'):
				return
			elif (should_overwrite == 'y'):
				break

	wifi_name = input('WiFi name: ')
	wifi_password = input('WiFi password: ')
	server_address = input('Server address: ')

	with open(secrets_file_path, 'w') as secrets_file:
		secrets_file.write(f'const static char wifiName[] = "{wifi_name}";\n')
		secrets_file.write(f'const static char wifiPassword[] = "{wifi_password}";\n')
		secrets_file.write(f'const static char serverAddress[] = "{server_address}";\n')
		secrets_file.write(f'const static uint16_t serverPort = 5341;\n')


def make_upload_file():
	upload_file_path = 'scripts/upload.sh'

	if (os.path.isfile(upload_file_path)):
		should_overwrite = ''

		while (should_overwrite not in ('y', 'n')):
			should_overwrite = input('Upload file already exists. Would you like to overwrite it? [y/n]: ')
			if (should_overwrite == 'n'):
				return
			elif (should_overwrite == 'y'):
				break

	board_list_output = subprocess.run(['arduino-cli', 'board', 'list', '--format', 'json'], capture_output=True, text=True)
	port_list = json.loads(board_list_output.stdout)
	boards_list = [port for port in port_list if 'matching_boards' in port]

	if len(boards_list) < 1:
		print('No boards connected, can\'t generate script.')
		return

	first_board = boards_list[0]
	first_address = first_board['port']['address']

	with open(upload_file_path, 'w') as upload_script:
		upload_script.write(f'#!/usr/bin/env bash\n')
		upload_script.write(f'arduino-cli upload -p {first_address} src/client/client.ino\n')


if __name__ == '__main__':
	make_secrets_file()
	make_upload_file()
	print('Done configuration.')


