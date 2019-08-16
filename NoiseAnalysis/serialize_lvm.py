import lvm_read
import numpy as np
import sys
from os import path

if __name__== '__main__':
	if len(sys.argv) < 2:
		print("Filename not given")
		sys.exit(-1)
	filename = sys.argv[1]
	metadata_filename = filename[:-4] + '_metadata.txt'
	data_filename = filename[:-4] + '_serialized.txt'
	if path.exists(metadata_filename) and path.exists(data_filename):
		print("Files are already done")
		sys.exit(0)
	print(filename)
	float_formatter = lambda x: "%.2f" % x
	np.set_printoptions(formatter={'float': '{: 0.5f}'.format})
	try:
		lvm = lvm_read.read(filename, read_from_pickle=False)
	except IOError as e:
		print(e)
		sys.exit(-1)
	#print lvm.keys()
	lvm_metadata = []
	if lvm.has_key('Fs(MHz)'):
		lvm_metadata.append(float(lvm['Fs(MHz)']))
	else:
		#np.insert(lvm_metadata, len(lvm_metadata)-1, 0)
		np.asarray(0)

	np.savetxt(metadata_filename, np.asarray(lvm_metadata), fmt='%1.4f')
	np.savetxt(data_filename, lvm[0]['data'], fmt='%1.4f')
	sys.exit(0)