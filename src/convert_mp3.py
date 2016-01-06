#!/usr/bin/env python

import os, sys, subprocess

SRC_ROOT = sys.argv[1]
DST_ROOT = sys.argv[2]

def format_size(n):
	for unit in (prefix + 'B' for prefix in [''] + list('kMGTPEZ')):
		if abs(n) < 1000.0:
			return '{0:3.1f}{1}'.format(n, unit)
		n /= 1000.0
	return '{0:3.1}YB'.format(n)

def printv(message):
	pass

total_num = 0
total_ins = 0
total_outs = 0

for root, dirs, files in os.walk(SRC_ROOT):
	r = os.path.relpath(root, SRC_ROOT)

	try:
		os.mkdir(os.path.join(DST_ROOT, r))
	except OSError:
		pass

	for filename in files:
		f = os.path.join(r, filename)
		inf = os.path.join(SRC_ROOT, f)
		outf = os.path.join(DST_ROOT, os.path.splitext(f)[0] + '.mp3')

		ret = subprocess.call([
			'lame',
			'--quiet',
			'--preset', '128',
			inf,
			outf,
		])

		if ret == 0:
			ins = os.stat(inf).st_size
			outs = os.stat(outf).st_size

			total_num += 1
			total_ins += ins
			total_outs += outs

			printv('{0:40} {1:7} -> {2:10} {3:d}%'.format(f,
				format_size(ins),
				format_size(outs),
				ins / outs))
		else:
			printv('{0:40} Failed!'.format(f))

printv('')
printv('{0:40} {1:7} -> {2:10} {3:d}%'.format(
		'Finished converting {0} files'.format(total_num),
		format_size(total_ins),
		format_size(total_outs),
		total_ins / total_outs))

