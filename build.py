import os
import nil.build, nil.environment, nil.setup

project = 'theodwalha'

nil.environment.unix_like()

builder = nil.build.builder(project)
for library in ['boost_system', 'boost_thread', 'boost_filesystem', 'ail', 'z', 'dl']:
	builder.library(library)

if builder.program():
	builder.link_static_library()
	nil.setup.include(project)
	nil.setup.library(builder.library)
