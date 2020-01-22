#ifndef __MAIN_PAGE_DATA_H__
#define __MAIN_PAGE_DATA_H__
#include <Arduino.h>

String PAGE_HEADER = "<!doctype html>"
"<html lang='en'>"
	"<head>"
		"<meta charset='utf-8'>"
		"<meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'>"
		"<link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/css/bootstrap.min.css'>"
	"</head>"
	"<body>";

String PAGE_NAVIGATION = "<nav class='navbar navbar-expand-lg navbar-light bg-light'>"
				"<a class='navbar-brand' href='#'>Serial Remote</a>"
				"<button class='navbar-toggler' type='button' data-toggle='collapse' data-target='#navbarNavAltMarkup' aria-controls='navbarNavAltMarkup' aria-expanded='false' aria-label='Toggle navigation'>"
					"<span class='navbar-toggler-icon'></span>"
				"</button>"
				"<div class='collapse navbar-collapse' id='navbarNavAltMarkup'>"
					"<div class='navbar-nav'>"
						"<a class='nav-item nav-link' href='/monitor'>Monitor</a>"
						"<a class='nav-item nav-link' href='/upload'>Update</a>"
					"</div>"
				"</div>"
			"</nav>";

String PAGE_BODY_START = "<div class='container-fluid' style='padding-top: 15px'>";

String PAGE_BODY_SCRIPTS = "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
		"<script src='https://cdn.jsdelivr.net/npm/popper.js@1.16.0/dist/umd/popper.min.js'></script>"
		"<script src='https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/js/bootstrap.min.js'></script>";

String PAGE_FOOTER = "</body>"
"</html>";

#endif /* __MAIN_PAGE_DATA_H__ */