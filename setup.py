from distutils.core import setup

setup(name='zebra',
	  description="""Translates REST HTTP-Request into the XRAP protocol.""",
	  version='0.1',
	  packages=['zebra'],
	  package_dir={'': 'bindings/python'},
)
