#include <stdio.h>

char *ats_ut_str = "###TEST%004d - 47.1003.1 - ba_infra_adjacent_\n"
"use constant TEST%004d_DESCR => TEST_PRE_DESCR.\"\";\n"
"use constant TEST%004d_ID => TEST_PRE_ID.\".\";\n"
"use constant TEST%004d_CFG => CFG_PATH.TEST%004d_DESCR.\".cfg\";\n"
"use constant TEST%004d_NOTES => \"\";\n"
"\n"
"sub init_%004d {\n"
"}\n"
"\n"
"sub test%004d\n"
"{\n"
"	my $pass = BA_INFRA_TEST_FAILED;\n"
"	my $test_descr = TEST%004d_DESCR;\n"
"	my $url = \"http://\".VIP_1.\"/alpha.html\";\n"
"	my $srch_patt = \"ABHIJKLMNOPQRSTUVWXYZ\";\n"
"	my $srch_inx = 0;\n"
"	my $real_patt;\n"
"\n"
"	my $ua = LWP::UserAgent->new;\n"
"\n"
"	my $req = HTTP::Request->new(GET => $url);\n"
"	$req->header('Accept' => '*/*');\n"
"	$req->header('Cache-Control' => 'no-cache');\n"
"\n"
"\n"
"	ats_log(BA_INFRA_INFO_STR.\"Request URL: [\".$url.\"]\");\n"
"	my $resp = $ua->request($req);\n"
"	if (is_valid_http_resp($resp)) {\n"
"		$real_patt = substr($resp->decoded_content,\n"
"						$srch_inx, length($srch_patt));\n"
"		if ($srch_patt eq $real_patt) {\n"
			"$pass = BA_INFRA_TEST_PASSED;\n"
"		}\n"
"	} else {\n"
	"	ats_log(BA_INFRA_ERR_STR.\n"
"			\"HTTP GET error code: \", $resp->code, \"\\n\");\n"
"		ats_log(BA_INFRA_ERR_STR.\n"
"			\"HTTP GET error message:\\n\", $resp->message, \"\n\");\n"
"	}\n"
"	\n"
"	return ($pass, $test_descr);\n"
"}\n"
"\n"
"sub cleanup_%004d {\n"
"}\n";

char *ats_ut_add_str = 
"ba_infra_ut_addtest(TEST%04d_ID, TEST%04d_DESCR,"
"TEST%04d_CFG, TEST%04d_NOTES, \\&test%04d, \\&init_%04d, \\&cleanup_%04d)\n";

int main()
{
	int i;

	for (i = 1; i <= 30; i++)
		printf(ats_ut_str, i, i ,i ,i, i, i, i, i, i, i, i);
	for (i = 1; i <= 30; i++)
		printf(ats_ut_add_str, i, i ,i ,i, i, i, i);
}
