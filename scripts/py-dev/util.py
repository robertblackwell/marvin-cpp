def run(cmd, where=None):
	if where is None:
		print("run: [{}] ".format(cmd))
	else:
		print("run: [{}] where = {} ".format(cmd, where))
