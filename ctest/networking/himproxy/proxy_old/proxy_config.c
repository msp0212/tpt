/*
 * This file contains configuration reading part
 */
/*System Headers*/
#include<string.h>
/*Common Header*/
#include<jsf_conf_if.h>
/*Local Headers*/
#include<proxy_config.h>
#include<proxy_globals.h>

static int get_proxy_config(void *fp, char *proxy_sec_name);
static int free_proxy_config();
int get_config(char *conf_file)
{
	void *fp = NULL;
	int ret = 0;

	if (conf_file == NULL) {
		fprintf(stderr, "%s : Invalid Params received !!!\n",
					__FUNCTION__);
		return -1;
	}
	if (jsf_conf_load(conf_file, 0, &fp) < 0) {
		fprintf(stderr, "%s : Error in jsf_conf_load for [%s]\n",
					__FUNCTION__, conf_file);
			return -1;
	}
	if (get_proxy_config(fp, PROXY_SECTION_NAME) < 0) {
		fprintf(stderr, "%s : Error in get_proxy_config !!!\n", __FUNCTION__);
		ret = -1;
		goto exit;
	}
exit:
	jsf_conf_unload(fp);
	return ret;
}

int free_config() 
{
	int ret = 0;
	ret = free_proxy_config();
	return 0;
}
static int get_proxy_config(void *fp, char *proxy_sec_name) 
{
	char *str = NULL;
	if (fp == NULL || proxy_sec_name == NULL) {
		fprintf(stderr, "%s : Invalid Params received !!!\n",
					__FUNCTION__);
		return -1;
	}

	str = GetStrConf(fp, proxy_sec_name, "ListenIP");
	if (str == NULL) {
		strncpy(g_proxy_config.listen_ip, str, 
				sizeof(g_proxy_config.listen_ip));
	} else {
		strncpy(g_proxy_config.listen_ip, "0.0.0.0", 
				sizeof(g_proxy_config.listen_ip));
	}
	free(str);str = NULL;
	fprintf(stderr, "%s : ListenIP [%s]\n", __FUNCTION__, 
				g_proxy_config.listen_ip);
	
	g_proxy_config.listen_port = GetIntConf(fp, proxy_sec_name,
						"ListenPort");
	if (g_proxy_config.listen_port < 0) {
		g_proxy_config.listen_port = 9001;
	}
	fprintf(stderr, "%s : ListenPort [%d]\n",
			__FUNCTION__, g_proxy_config.listen_port);

	g_proxy_config.child_count = GetIntConf(fp, proxy_sec_name,
						"ChildCount");
	if (g_proxy_config.child_count < 0) {
		g_proxy_config.child_count = 1;
	}
	fprintf(stderr, "%s : ChildCount [%d]\n", __FUNCTION__,
			g_proxy_config.child_count);

	g_proxy_config.log_level = GetIntConf(fp, proxy_sec_name, "LogLevel");
	if (g_proxy_config.log_level < 0) {
		g_proxy_config.log_level = 4;
	}
	fprintf(stderr, "%s : LogLevel [%d]\n", __FUNCTION__, 
					g_proxy_config.log_level);

	g_proxy_config.workq_parallelism = GetIntConf(fp, proxy_sec_name, "WorkqParallelism");
	if (g_proxy_config.workq_parallelism < 0) {
		g_proxy_config.workq_parallelism = 25;
	}
	fprintf(stderr, "%s : WorkqParallelism [%d]\n", __FUNCTION__,
			g_proxy_config.workq_parallelism);

	g_proxy_config.workq_thread_timeout = GetIntConf(fp, proxy_sec_name, "WorkqThreadTimeout");
	if (g_proxy_config.workq_thread_timeout < 0) {
		g_proxy_config.workq_thread_timeout = 10;
	}
	fprintf(stderr, "%s : WorkqThreadTimeout [%d]\n", __FUNCTION__,
			g_proxy_config.workq_thread_timeout);
	
	g_proxy_config.workq_length = GetIntConf(fp, proxy_sec_name, "WorkqLength");
	if (g_proxy_config.workq_length < 0) {
		g_proxy_config.workq_length = 0;
	}
	fprintf(stderr, "%s : WorkqLength [%d]\n", __FUNCTION__,
			g_proxy_config.workq_length);
	g_proxy_config.ip_transparency = GetIntConf(fp, proxy_sec_name, "IPTransparency");
	if (g_proxy_config.ip_transparency < 0) {
		g_proxy_config.ip_transparency = 0;
	}
	fprintf(stderr, "%s : IPTransparency [%d]\n", __FUNCTION__,
			g_proxy_config.ip_transparency);

	return 0;
}

static int free_proxy_config()
{
	return 0;
}
