/*
 * Copyright (C) 2019 Kacper Kołodziej <kacper@kolodziej.it> https://kacperkolodziej.pl/
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <sys/socket.h>

extern ngx_module_t ngx_http_mptcp_module;

static ngx_int_t ngx_http_mptcp_variable(ngx_http_request_t *r,
		ngx_http_variable_value_t *v, uintptr_t data)
{
	int fd, result, mptcp_enabled;
#define MPTCP_ENABLED 42
	socklen_t mptcp_enabled_len = sizeof(mptcp_enabled);

	fd = r->connection->fd;
	result = getsockopt(fd, SOL_TCP, MPTCP_ENABLED, &mptcp_enabled, &mptcp_enabled_len);
	if (result == 0) {
		if (mptcp_enabled) {
			v->len = 2;
			v->data = (u_char*)"on";
			v->not_found = 0;
			v->valid = 1;
			v->no_cacheable = 1;
		} else {
			v->len = 3;
			v->data = (u_char*)"off";
			v->not_found = 0;
			v->valid = 1;
			v->no_cacheable = 1;
		}
	} else {
		v->not_found = 1;
	}

	return NGX_OK;
}

static ngx_str_t ngx_http_mptcp_enabled_name = ngx_string("mptcp_enabled");

static ngx_int_t ngx_http_mptcp_init(ngx_conf_t* cf) {
	ngx_http_variable_t *var;
	var = ngx_http_add_variable(cf, &ngx_http_mptcp_enabled_name,
			NGX_HTTP_VAR_NOCACHEABLE);
	if (var == NULL) {
		return NGX_ERROR;
	}

	var->get_handler = ngx_http_mptcp_variable;

	return NGX_OK;
}

static ngx_http_module_t ngx_http_mptcp_module_ctx = {
	ngx_http_mptcp_init,			/* preconfiguration */
	NULL,					/* postconfiguration */

	NULL,					/* create main configuration */
	NULL,					/* init main configuration */

	NULL,					/* create server configuration */
	NULL,					/* merge server configuration */

	NULL,					/* create location configuration */
	NULL 					/* merge location configuration */
};

ngx_module_t ngx_http_mptcp_module = {
	NGX_MODULE_V1,
	&ngx_http_mptcp_module_ctx,		/* module context */
	NULL,					/* module directives */ 
	NGX_HTTP_MODULE,			/* module type */
	NULL,					/* init master */
	NULL,					/* init module */
	NULL,					/* init process */
	NULL,					/* init thread */
	NULL,					/* exit thread */
	NULL,					/* exit process */
	NULL,					/* exit master */
	NGX_MODULE_V1_PADDING
};
