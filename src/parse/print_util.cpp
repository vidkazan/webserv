//
// Created by Feyd Rodney on 3/29/22.
//

#include "serverConfig.hpp"

void ServerConfig::_printConfigurations() {
	cout << "======================COUT_CONFIGURATIONS======================\n" ;
	cout << "rawip: " << this->listen->rawIp << endl;
	cout << "serv name: " << this->server_name << endl;
	cout << "client_body_buffer_size: " << this->client_body_buffer_size << endl;
	cout << "index: " << this->index << endl;
	cout << "autoindex: ";
	if (this->autoindex == true)
		cout << "on";
	else
		cout << "off";
	cout << endl;
	if (this->listen) {
		cout << "listen ip : " << this->listen->rawIp << endl;
		if (this->listen->port) {
			cout << "listen port: " << this->listen->port << endl;
		}
	}
	if (!this->root.empty())
		cout << "serv root : " << this->root << endl;
	if (!this->error_pages.empty()) {
		cout << "serv err_pages:" << endl;
		for (map<int, string>::iterator it = this->error_pages.begin();
			 it != this->error_pages.end(); it++)
			cout << "code: " << it->first << " file: " << it->second << endl;
	}
	if (!this->locations.empty()) {
		vector<LocationConfig *>::iterator b = this->locations.begin();
		vector<LocationConfig *>::iterator e = this->locations.end();
		while (b != e) {
			if (!(*b)->name.empty())
				cout << "***********location name: '" << (*b)->name << "'***********" << endl;
			if (!(*b)->allow_methods.empty())
				cout << "location methods:\t" << (*b)->allow_methods << endl;
			if (!(*b)->root.empty())
				cout << "location root:\t" << (*b)->root << endl;
			if (!(*b)->index.empty())
				cout << "location index:\t" << (*b)->index << endl;
			if (!(*b)->cgi_path.empty())
				cout << "location cgi_path:\t" << (*b)->cgi_path << endl;
			if (!(*b)->cgi_extension.empty())
				cout << "location cgi_extension:\t" << (*b)->cgi_extension << endl;
			cout << "location autoindex:\t";
			if ((*b)->autoindex == true)
				cout << "on";
			else
				cout << "off ";
			cout << endl;
			if (!(*b)->allow_methods.empty()) {
				cout << "location allow_meth:\t" << (*b)->allow_methods << endl;
			}
			if (!(*b)->error_pages.empty()) {
				cout << "location err_pages:" << endl;
				for (map<int, string>::iterator it = (*b)->error_pages.begin();
					 it != (*b)->error_pages.end(); it++)
					cout << "code: " << it->first << " file: " << it->second << endl;
			}
			if (!(*b)->redirect.empty())
				cout << "location redirect: " << (*b)->redirect << endl;
			b++;
		}
	}
}