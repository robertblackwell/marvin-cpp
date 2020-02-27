#!/usr/bin/env python
import requests
import sys
import json
import ecm
import argparse
import datetime
import pprint
pp = pprint.PrettyPrinter(indent=4)

__version__ = "0.3.5"

debug = False
batch_mode = True
logfile = None
router_url = ecm.routers_url
group_url = ecm.groups_url
headers = ecm.headers

def router_get_one(router_id):
	req = requests.get(routers_url + router_id + "/", headers=headers)
	# req = requests.api.request()
	status_code = req.status_code
	if debug: pp.pprint(status_code)
	if status_code < 200 or status_code >= 300:
		return None
	try:
		resp = req.json()
	except:
		return None
	
	return resp

def group_get_one(group_url):
	url = group_url
	req = requests.get(url, headers=headers)
	status_code = req.status_code
	if debug: print("get_a_group : type(status_code): " + str(type(status_code)) )
	if status_code < 200 or status_code >= 300 :
		# raise RuntimeError("bad status code on request")
		return None
	try:
		r = req.json()
	except:
		# raise RuntimeError("json parse error")
		return None
	return r

def router_set_group(router_id, group_url):
	if debug: print(router_id,"  ",group_url)
	g = group_get_one(group_url)
	if g is None:
		raise RuntimeError("failed to get group record")
	if debug: pp.pprint(g)
	firmware_id_url = g['target_firmware']
	group_id_url = g['resource_url']
	account_url = g['account']
	payload = {
		'account' : account_url,
		'group' : group_id_url,
	}
	if debug: pp.pprint(group_id_url)
	if debug: pp.pprint(json.dumps(payload)) 
	
	req = requests.put(
		router_url + router_id + "/", 
		json = payload, 
		headers=headers
	)
	status_code = req.status_code
	if debug: pp.pprint(status_code)
	if status_code < 200 or status_code >= 300:
		raise RuntimeError("bad status code from request")
		return None
	try:
		resp = req.json()
	except:
		raise RuntimeError("error parsing json")
		return None

	return resp

def mk_group_url(id):
	return group_url+str(id)+"/"

def get_target_group_v2(curernt_group):
	t = [
			#Test Logic
			{
				"name"  : "TestGroup1",
				"id"	: 154505,
				"msg" :"TestGroup1 -> TestGroup2",
				"already" : false
			},
			{
				"name": "TestGroup2",
				"id" : 83225,
				"msg" : "TestGroup2 -> TestGroup1",
				"already" : false
			},

			#ATT 850 Logic
			{
				"name"  : "SBUX ATT 850 6.4.2 PROD",
				"id"	: 77266,
				"msg" : "6.4.2 -> 7.1.20 ATT PROD"
				"already" : false
			},
			{
				"name"  : "SBUX ATT 850 6.4.2 STAGING",
				"id"	: 83224,
				"msg" : "6.4.2 -> 7.1.20 ATT STAG",
				"already" : false
			},
			{
				"name"  : "SBUX ATT 850 7.1.20 PROD",
				"id"	: 152132,
				"msg" : "7.1.20 ATT PROD",
				"already" : true
			},
			{
				"name"  : "SBUX ATT 850 7.1.20 STAGING",
				"id"	: 154477,
				"msg" : "7.1.20 ATT STAGING",
				"already" : true
			},

			#VZW 850 Logic
			{
				"name"  : "SBUX VZW 850 6.4.2 PROD",
				"id"	: 77267,
				"msg" : "6.4.2 -> 7.1.20 VZW PROD",
				"already" : false
			},
			{
				"name"  : "SBUX VZW 850 6.4.2 STAGING",
				"id"	: 83225,
				"msg" : "6.4.2 -> 7.1.20 VZW STAG",
				"already" : false
			},
			{
				"name"  : "SBUX VZW 850 7.1.20 PROD",
				"id"	: 151726,
				"msg" : "7.1.20 VZW PROD",
				"already" : true
			},
			{
				"name"  : "SBUX VZW 850 7.1.20 STAGING",
				"id"	: 154476,
				"msg" : "7.1.20 VZW STAGING",
				"already" : true
			}
	]

	for entry in t:
		if (current_group == mk_group_url(entry["id"])) :
			if (entry['already']):
				return False, None, "Device already has been upgraded | " + entry["msg"]
			else:
				return True, mk_group_url(entry["id"]), entry["msg"]

	# did not find
	return False, None, "Invalid Group ID " + str(current_group)


def get_target_group(current_group):
	#Source Groups
	#ATT
	SBUX_ATT_850_6_4_2_PROD = 77266
	SBUX_ATT_850_6_4_2_STAGING = 83224
	#VZW
	SBUX_VZW_850_6_4_2_PROD = 77267
	SBUX_VZW_850_6_4_2_STAGING = 83225
	
	#Target Groups
	#ATT
	SBUX_ATT_850_7_1_20_PROD = 152132
	SBUX_ATT_850_7_1_20_STAGING = 154477
	#VZW
	SBUX_VZW_850_7_1_20_PROD = 151726
	SBUX_VZW_850_7_1_20_STAGING = 154476

	#Test Groups 
	TestGroup1 = 154505
	TestGroup2 = 83225

	#Test Logic
	if (current_group == mk_group_url(TestGroup1)):
		msg = "TestGroup1 -> TestGroup2"
		return True, mk_group_url(TestGroup2), msg
	if (current_group == mk_group_url(TestGroup2)):
		msg = "TestGroup2 -> TestGroup1"
		return True, mk_group_url(TestGroup1), msg
	
	#ATT 850 Logic
	if (current_group == mk_group_url(SBUX_ATT_850_6_4_2_PROD)):
		msg = "6.4.2 -> 7.1.20 ATT PROD"
		return True, mk_group_url(SBUX_ATT_850_7_1_20_PROD), msg
	if (current_group == mk_group_url(SBUX_ATT_850_6_4_2_STAGING)):
		msg = "6.4.2 -> 7.1.20 ATT STAG"
		return True, mk_group_url(SBUX_ATT_850_7_1_20_STAGING), msg
	if (current_group == mk_group_url(SBUX_ATT_850_7_1_20_PROD)):
		msg = "7.1.20 ATT PROD"
		return False, None, "Device has already been upgraded to target version |" + msg
	if (current_group == mk_group_url(SBUX_ATT_850_7_1_20_STAGING)):
		msg = "7.1.20 ATT STAGING"
		return False, None, "Device has already been upgraded to target version |" + msg
	#VZW 850 Logic
	if (current_group == mk_group_url(SBUX_VZW_850_6_4_2_PROD)):
		msg = "6.4.2 -> 7.1.20 VZW PROD"
		return True, mk_group_url(SBUX_VZW_850_7_1_20_PROD), msg
	if (current_group == mk_group_url(SBUX_VZW_850_6_4_2_STAGING)):
		msg = "6.4.2 -> 7.1.20 VZW STAG"
		return True, mk_group_url(SBUX_VZW_850_7_1_20_STAGING), msg
	if (current_group == mk_group_url(SBUX_VZW_850_7_1_20_PROD)):
		msg = "7.1.20 VZW PROD"
		return False, None, "Device has already been upgraded to target version |" + msg
	if (current_group == mk_group_url(SBUX_VZW_850_7_1_20_STAGING)):
		msg = "7.1.20 VZW STAGING"
		return False, None, "Device has already been upgraded to target version |" + msg
	return False, None, "Invalid Group ID " + str(current_group)

def query_router(router_id):
	req = requests.get(router_url+router_id+"/", headers=headers)
	status_code = req.status_code
	if ((status_code < 200) or (status_code >= 300)):
		# print("Before raise")
		raise RuntimeError("API call failed status: "+str(status_code))
	else:
		# print("after else")
		routers_resp = req.json()
		if debug: pp.pprint(routers_resp)
		state = routers_resp["state"]
		if debug: pp.pprint(state)
		group = routers_resp["group"]
		if debug: print (str(group))
		store = routers_resp["name"]
		if debug: pp.pprint(store)
		# print("query return")
		return [(state==u'online'),store,group]

def valid_router_id(arg):
	isnumeric = all(i.isdigit() for i in arg) #checking each character of arg is numeric
	if isnumeric:
	#if (len(arg)==6) and isnumeric:
		if debug: print ("Router ID is valid "+arg)
		return arg
	usage_error("Router ID is invalid!! "+arg)
	return None

def usage_error(message):
	if batch_mode:
		print("Fail! usage error: "+message)
	else:
		print(message)
		print("""
	PURPOSE:
		Move a Cradlepoint router to a new configuration group.
		This script will determine which group a Cradlepoint device is currently assigned to.
		Based on the current group ID, the script will determine the appropriate target grou ID.
		Finally, it will move the device into the appropriate target group.

	SYNTAX:
		ncm_move.py <router_id>
	
		<router_id>	This is a unique identifier which refers to an individual Cradlepoint modem. (Example 164270)

	EXAMPLES:
		ncm_move.py 164270
		ncm_move.py --list=file.txt
		
		""")

def group_target_error(store,router_id,current_group,message):
	print("Fail    |Router:"+router_id+" |Store:"+store+" |Current Group:"+current_group[45:50]+" |"+message)
	global logfile
	logfile.write("Fail    |Router:"+router_id+" |Store:"+store+" |Current Group:"+current_group[45:50]+" |"+message+"\n")

def group_error(store,router_id,current_group,message):
	print("Fail    |Router:"+router_id+" |Store:"+store+" |Current Group:"+current_group[45:50]+" |"+message)
	global logfile
	logfile.write("Fail    |Router:"+router_id+" |Store:"+store+" |Current Group:"+current_group[45:50]+" |"+message+"\n")

def success(store,router_id,group_current,group_target,msg):
	line="Success |Router:"+router_id+" |Store:"+store+" |Current Group:"+group_current[45:50]+" |Target Group:"+group_target[45:50]+" |"+msg
	print(line)
	global logfile
	logfile.write(line+"\n")

def group_put_error(store,router_id,group_current,group_target):
	print("Fail    |Router:"+router_id+" |Store:"+store+" |Current Group:"+group_current[45:50]+" |Target Group: "+group_target[45:50]+" |PUT query failed")
	global logfile
	logfile.write("Fail    |Router:"+router_id+" |Store:"+store+" |Current Group:"+group_current[45:50]+" |Target Group: "+group_target[45:50]+" |PUT query failed"+"\n")

def doit(arg, debug_flag, dryrun_flag):
	router_id = valid_router_id(arg)
	if router_id is None: return
	try:
		status, store, group_current = query_router(router_id)
		if not status:
			print("Fail    |Router:"+router_id+" |Store:"+store+" |Current Group:"+group_current[45:50]+" |Device is offline")
			global logfile
			logfile.write("Fail    |Router:"+router_id+" |Store:"+store+" |Current Group:"+group_current[45:50]+" |Device is offline"+"\n")
			return
		is_ok, group_target, msg = get_target_group(group_current)
		if debug: print("is_ok: " + str(is_ok) + " group_target is: " + group_target + " msg: " + msg)
		if is_ok :
			try:
				if debug: pp.pprint(dryrun_flag)
				if not dryrun_flag:
					router_set_group(router_id,group_target)
				else:
					pass
					if debug:print("Performing Dryrun...")
				success(store,router_id,group_current,group_target,msg)
			except:
				group_put_error(store,router_id,group_current,group_target)
				return
		else:
			group_target_error(store,router_id,group_current,msg)
			return
	except:
		print("Fail    |Router:"+router_id+" |Query failed | Router ID may not exist")

def doitonafile(filename,debug_flag,dryrun_flag):
	if debug: print("filename: " + filename)
	f = open(filename,'r')
	if debug: pp.pprint(f)
	data=f.read()
	if debug: pp.pprint(data)
	list=data.split("\n")
	if debug: pp.pprint(list)
	for ix,router in enumerate(list):
		if len(router.strip()) > 0:
			if debug: print(str(ix)+ " " + router)
			# print("ix:%d router:%s",ix,router)
			doit(router,debug_flag,dryrun_flag)

def main():
	parser = argparse.ArgumentParser(description="This script's purpose is to upgrade Cradlepoint routers. This script will determine which group a Cradlepoint device is currently assigned to. It will then determine the appropriate group the device needs to be moved into. Finally, it will move the device into the appropriate target group.")
	parser.add_argument("router_id", 	nargs="?", 		help="Router Id")
	parser.add_argument('-v','--version', 	action="store_true", 	help="Prints the version number.")
	parser.add_argument('--list', 		dest='infile_path', 	help='Path to input file, each line has arguments for command. Cannot parse both a file and a string')
	parser.add_argument('--log', 		dest='logfile_path', 	help='Path to log file, default is ncm_results_YYYYMMDDHHMMSS.txt in the working directory')
	parser.add_argument('--debug', 		action="store_true", 	help="Prints out the command to be executed rather than execute the command, to help problem solve")
	parser.add_argument('--dryrun', 	action="store_true", 	help="Does NOT execute group changes but does provide logs")
	args = parser.parse_args()

	if debug: pp.pprint(args) 

	if args.version:
		print(__version__)
		sys.exit(0)

	if args.router_id is not None and args.infile_path is not None:
		print("Cannot handle both infile_path and router_id")
		sys.exit(1)

	if args.logfile_path:
		logpath = args.logfile_path
	else:
		ts = datetime.datetime.now().isoformat()
		if debug: pp.pprint(ts)
		logpath = "ncm_results_"+ts
		if debug: pp.pprint(logpath)
		global logfile
		logfile = open(logpath, "w")
	if debug: pp.pprint(logfile)

	if args.router_id is not None: 
		if debug: print("pretend that we processed the router_id: "+args.router_id)
		doit(args.router_id, args.debug is not None, args.dryrun)

	if args.infile_path is not None: 
		if debug: print("pretend that we processed the infile_path: "+args.infile_path)
		doitonafile(args.infile_path, args.debug is not None, args.dryrun)


if __name__ == "__main__":
	main()
