#!/usr/bin/env python3

import os

secrets_file_path = 'src/Client/secrets.h'

if (os.path.isfile(secrets_file_path)):
	should_overwrite = ''

	while (should_overwrite not in ('y', 'n')):
		should_overwrite = input('Secrets file already exists. Would you like to overwrite it? [y/n]: ')
		if (should_overwrite == 'n'):
			exit()
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

print('Done configuration.')